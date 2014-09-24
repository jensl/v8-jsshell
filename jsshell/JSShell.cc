/* -*- mode: c++ -*- */
/*

  Copyright 2013 Jens Lindström

  Licensed under the Apache License, Version 2.0 (the "License"); you may not
  use this file except in compliance with the License.  You may obtain a copy of
  the License at

       http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
  License for the specific language governing permissions and limitations under
  the License.

*/

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <map>

#include "Base.h"
#include "Initialize.h"

#include "api/GC.h"
#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/builtin/Module.h"
#include "jsshell/ArgumentsParser.h"
#include "utilities/Anchor.h"

#include "libplatform/libplatform.h"

void stop() {
  int stop_here = 0;
  (void)stop_here;
}

std::string String(v8::Handle<v8::String> value, const char* if_empty = "") {
  if (value.IsEmpty() || value->Length() == 0)
    return std::string(if_empty);

  v8::String::Utf8Value utf8(value);
  return std::string(*utf8, utf8.length());
}


void PrintException(v8::TryCatch& try_catch) {
  v8::Handle<v8::Message> message(try_catch.Message());

  if (message.IsEmpty()) {
    fprintf(stderr, "Exception: %s\n", String(try_catch.Exception()->ToString()).c_str());
    return;
  }

  fprintf(stderr, "%s:%d: %s\n",
	  String(message->GetScriptResourceName()->ToString()).c_str(),
	  message->GetLineNumber(),
	  String(message->Get()).c_str());

  v8::Handle<v8::StackTrace> stacktrace(message->GetStackTrace());

  if (!stacktrace.IsEmpty() && stacktrace->GetFrameCount() != 0) {
    fprintf(stderr, "Stacktrace:\n");

    for (int index = 0; index < stacktrace->GetFrameCount(); ++index) {
      v8::Handle<v8::StackFrame> stackframe(stacktrace->GetFrame(index));
      v8::Handle<v8::String> function(stackframe->GetFunctionName());
      std::string context;

      if (function.IsEmpty() || function->Length() == 0) {
        context = "<global code>";
      } else {
        context = String(function);
        context += "()";
      }

      fprintf(stderr, "  %s at %s:%d\n",
	      context.c_str(),
	      String(stackframe->GetScriptName()).c_str(),
	      stackframe->GetLineNumber());
    }
  }
}

namespace {

class ReleaseInstance {
 public:
  void operator() (modules::builtin::Module::Instance* instance) {
    modules::builtin::Module::Release(instance);
  }
};

}

int Main(const std::vector<std::string>& argv,
         const std::map<std::string, std::string>& environ) {
  struct rlimit core;
  core.rlim_cur = core.rlim_max = RLIM_INFINITY;
  if (setrlimit(RLIMIT_CORE, &core) == -1) {
    perror("rlimit failed");
    return EXIT_FAILURE;
  }

  ArgumentsParser parser;

  parser.AddAlias('E', "enable");
  parser.AddAlias('D', "disable");
  parser.AddAlias('e', "evaluate");
  parser.Parse(argv);

  v8::Platform* platform = v8::platform::CreateDefaultPlatform();

  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();

  v8::Isolate::CreateParams create_params;

  base::PreInitialize(create_params);
  api::PreInitialize(create_params);

  v8::Isolate* isolate = v8::Isolate::New(create_params);
  v8::Isolate::Scope isolate_scope(isolate);

  v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);

  base::Initialize();
  api::Initialize();

  std::map<std::string, bool> features;
  for (auto iter(parser.begin()); iter != parser.end(); ++iter) {
    if (iter->is_option()) {
      if (iter->name() == "enable")
        features.insert(std::make_pair(iter->value(), true));
      else if (iter->name() == "disable")
        features.insert(std::make_pair(iter->value(), false));
    }
  }

  v8::HandleScope handle_scope(CurrentIsolate());

  modules::builtin::Module::Instance* root =
      modules::builtin::Module::Root(features);

  utilities::Anchor<modules::builtin::Module::Instance, ReleaseInstance>
      anchor(root);

  api::Runtime::Select select(modules::builtin::Module::runtime(root));

  for (auto iter(parser.begin()); iter != parser.end(); ++iter) {
    if ((iter->is_option() && iter->name() == "evaluate") ||
        iter->is_argument()) {
      v8::TryCatch try_catch;

      if (iter->is_option())
        modules::builtin::Module::Eval(root, iter->value());
      else
        modules::builtin::Module::Load(root, iter->value());

      if (try_catch.HasCaught()) {
        PrintException(try_catch);
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}

