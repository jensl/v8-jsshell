namespace std {
class type_info;
}

#include <iostream>
#include <cstdio>

#include "Base.h"
#include "api/Class.h"
#include "api/Module.h"
#include "api/Runtime.h"
#include "modules/BuiltIn.h"
#include "modules/IO.h"
#include "modules/OS.h"
#include "modules/URL.h"
#include "modules/PostgreSQL.h"
#include "utilities/Formatter.h"
#include "utilities/File.h"
#include "utilities/CompileFile.h"

class Point : public api::Class {
 public:
  class Instance : public api::Class::Instance<Point> {
   public:
    ~Instance();

    double x, y;
  };

  Point();

  static Instance* constructor(Point*, double x, double y);
  static double sum(Point::Instance* point);
  static void offset(Point::Instance* point, double delta);

  static int get_x(Point::Instance* point) { return point->x; }
  static void set_x(Point::Instance* point, int x) { point->x = x; }

  static int get_y(Point::Instance* point) { return point->y; }
  static void set_y(Point::Instance* point, int y) { point->y = y; }
};

Point::Instance::~Instance() {
  std::cout << "Point::Instance deleted" << std::endl;
}

Point::Point()
    : Class("Point", &constructor) {
  AddMethod<Point>("sum", &sum);
  AddProperty<Point>("x", &get_x, &set_x);
  AddProperty<Point>("y", &get_y);
}

Point::Instance* Point::constructor(Point*, double x, double y) {
  Instance* instance = new Instance;

  instance->x = x;
  instance->y = y;

  return instance;
}

double Point::sum(Point::Instance* point) {
  return point->x + point->y;
}

void Point::offset(Point::Instance* point, double delta) {
  point->x += delta;
  point->y += delta;
}

v8::Handle<v8::String> ToString(std::string value) {
  return v8::String::New(value.c_str(), value.length());
}

std::string FromString(v8::Handle<v8::String> value,
                       const char* if_empty = "") {
  if (value.IsEmpty() || value->Length() == 0)
    return std::string(if_empty);

  v8::String::Utf8Value utf8(value);
  return std::string(*utf8, utf8.length());
}

v8::Handle<v8::Value> RunScript(v8::Handle<v8::Script> script) {
  if (script.IsEmpty())
    return v8::Undefined();

  v8::TryCatch try_catch;

  v8::Handle<v8::Value> result(script->Run());

  if (!try_catch.HasCaught())
    return result;
  else
    throw base::UncaughtException(try_catch);
}

class FileHandle {
 public:
  FileHandle(FILE* file)
      : file_(file) {
  }

  ~FileHandle() {
    if (file_)
      fclose(file_);
  }

  void close() {
    if (file_) {
      fclose(file_);
      file_ = NULL;
    }
  }

  FILE *operator-> () { return file_; }
  operator FILE* () { return file_; }

 private:
  FILE* file_;
};

v8::Handle<v8::Value> RunFile(std::string filename) {
  try {
    return RunScript(utilities::CompileFile(filename, ""));
  } catch (utilities::File::Error error) {
    printf("%s\n", error.message().c_str());
    return v8::Undefined();
  }
}

int main(int argc, char** argv) {
  v8::V8::Initialize();
  v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);

  v8::HandleScope handle_scope;

  api::Runtime runtime;

  runtime.Start();

  v8::Context::Scope context_scope(runtime.context());

  modules::BuiltIn builtin;
  builtin.AddToRuntime(runtime);

  modules::IO io;
  io.AddToRuntime(runtime);

  modules::OS os(true);
  os.AddToRuntime(runtime);

#if LIBCURL_SUPPORT
  modules::URL url;
  url.AddToRuntime(runtime);
#endif

#if POSTGRESQL_SUPPORT
  modules::PostgreSQL postgresql;
  postgresql.AddToRuntime(runtime);
#endif

  Point point;
  point.AddTo(builtin);

  try {
    v8::Handle<v8::Value> result(RunFile("test.js"));

    if (!result.IsEmpty() && !result->IsUndefined()) {
      v8::String::AsciiValue ascii(result);
      std::cout << *ascii << std::endl;
    }
  } catch (base::FileNotFound error) {
    printf("%s: file not found\n", error.filename().c_str());
  } catch (base::UncaughtException uncaught_exception) {
    v8::Handle<v8::Message> message(uncaught_exception.message());
    v8::String::Utf8Value utf8(message->Get());

    printf("%s:%d: %s\n",
           FromString(message->GetScriptResourceName()->ToString()).c_str(),
           message->GetLineNumber(),
           *utf8);

    v8::Handle<v8::StackTrace> stacktrace(uncaught_exception.message()->GetStackTrace());

    if (!stacktrace.IsEmpty()) {
      printf("stacktrace:\n");

      for (int index = 0; index < stacktrace->GetFrameCount(); ++index) {
        v8::Handle<v8::StackFrame> stackframe(stacktrace->GetFrame(index));
        v8::Handle<v8::String> function(stackframe->GetFunctionName());
        std::string context;

        if (function.IsEmpty() || function->Length() == 0) {
          context = "<global code>";
        } else {
          context = FromString(function);
          context += "()";
        }

        printf("  %s at %s:%d\n",
               context.c_str(),
               FromString(stackframe->GetScriptName()).c_str(),
               stackframe->GetLineNumber());
      }
    }
  }

  return 0;
}
