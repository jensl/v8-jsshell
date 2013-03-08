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

#include "Base.h"
#include "modules/BuiltIn.h"

#include <stdio.h>

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/builtin/Bytes.h"
#include "modules/builtin/Module.h"
#include "utilities/Formatter.h"
#include "utilities/FileDescriptor.h"

namespace modules {

BuiltIn::BuiltIn()
    : api::Module(kBuiltIn, "__builtin__")
    , bytes_(new builtin::Bytes)
    , module_(new builtin::Module) {
}

BuiltIn::~BuiltIn() {
  delete bytes_;
  delete module_;
}

void BuiltIn::ExtendObject(base::Object target) {
  AddFunction(target, "format", &format);
  AddFunction(target, "write", &write);
  AddFunction(target, "writeln", &writeln);
  AddFunction(target, "read", &read);
  AddFunction(target, "readln", &readln);
  AddFunction(target, "scoped", &scoped);
  AddFunction(target, "exit", &exit);

  bytes_->AddTo(target);
  module_->AddTo(target);
}

std::string BuiltIn::format(BuiltIn* module, std::string format,
                           const std::vector<base::Variant>& rest) {
  return utilities::Formatter().Format(format, rest);
}

void BuiltIn::write(BuiltIn* module, std::string format,
                   const std::vector<base::Variant>& rest) {
  if (rest.empty())
    ::fputs(format.c_str(), stdout);
  else
    ::fputs(utilities::Formatter().Format(format, rest).c_str(), stdout);

  ::fflush(stdout);
}

void BuiltIn::writeln(BuiltIn* module, Optional<std::string> format,
                     const std::vector<base::Variant>& rest) {
  if (format.specified())
    write(module, format.value(), rest);

  ::fputc('\n', stdout);
  ::fflush(stdout);
}

std::string BuiltIn::read(BuiltIn* module) {
  std::string result;
  char buffer[8172];

  while (!::feof(stdin) && !::ferror(stdin)) {
    size_t nread = ::fread(buffer, 1, sizeof buffer, stdin);
    if (nread > 0)
      result.append(buffer, nread);
  }

  return result;
}

std::string BuiltIn::readln(BuiltIn* module) {
  std::string result;

  while (!::feof(stdin) && !::ferror(stdin)) {
    int ch = ::fgetc(stdin);
    if (ch == EOF)
      break;
    result += ch;
    if (ch == '\n')
      break;
  }

  return result;
}

base::Variant BuiltIn::scoped(
    BuiltIn*, base::Object scope, base::Function function,
    const std::vector<base::Variant>& rest) {
  class Anchor {
   public:
    Anchor(base::Object scope)
        : scope_(scope) {
    }

    ~Anchor() {
      try {
        scope_.Call("close");
      } catch (...) {
        /* Ignore exceptions here.  Destructors called during stack unwind are
           not allowed to throw exception. */
      }
    }

   private:
    base::Object scope_;
  };

  Anchor anchor(scope);

  return function.Call(scope, rest);
}

void BuiltIn::exit(BuiltIn*, int status) {
  ::exit(status);
}

BuiltIn* BuiltIn::FromContext(v8::Handle<v8::Context> context) {
  return api::Module::FromContext<BuiltIn>(kBuiltIn, context);
}

}
