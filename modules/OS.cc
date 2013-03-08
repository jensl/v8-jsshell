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
#include "modules/OS.h"

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/os/Process.h"

namespace modules {

OS::OS(const Features& features)
    : api::Module(kOS, "OS")
    , process_(NULL) {
  if (!features.Disabled("OS.Process"))
    process_ = new os::Process(features);
}

OS::~OS() {
  delete process_;
}

void OS::ExtendObject(base::Object target) {
  if (process_)
    process_->AddTo(target);
}

void OS::ExtendRuntime(api::Runtime& runtime) {
  AddTo(runtime.GetGlobalObject());
  ExtendObject(GetObject());
}

OS* OS::FromContext(v8::Handle<v8::Context> context) {
  return api::Module::FromContext<OS>(kOS, context);
}

}
