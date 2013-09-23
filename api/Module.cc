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
#include "api/Module.h"
#include "api/Runtime.h"

namespace api {

Module::~Module() {
  template_.Dispose(v8::Isolate::GetCurrent());
}

Module::Module(int index, std::string name)
    : index_(index)
    , name_(name)
    , template_(v8::Isolate::GetCurrent(), v8::FunctionTemplate::New()) {
  function_template()->SetClassName(v8::String::New(name.c_str(), name.length()));
}

void Module::AddToRuntime(Runtime& runtime) {
  runtime.context()->SetEmbedderData(index_, v8::External::New(this));
  ExtendRuntime(runtime);
}

void Module::ExtendRuntime(Runtime& runtime) {
  ExtendObject(runtime.GetGlobalObject());
}

base::Object Module::GetObject() {
  return function_template()->GetFunction();
}

void Module::AddTo(base::Object target) {
  target.Put(name_, GetObject());
}

}
