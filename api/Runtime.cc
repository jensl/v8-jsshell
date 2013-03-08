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
#include "api/Runtime.h"

#include "v8.h"

namespace api {

Runtime::Select::Select(Runtime& runtime)
    : context_scope_(runtime.context()) {
}

Runtime::~Runtime() {
  context_.Dispose(v8::Isolate::GetCurrent());
}

void Runtime::Start() {
  context_ = v8::Context::New();
}

void Runtime::Stop() {
  context_.Dispose(v8::Isolate::GetCurrent());
}

base::Object Runtime::GetGlobalObject() {
  return context_->Global();
}

}
