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
#include "modules/Testing.h"

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/testing/Enumeration.h"
#include "modules/testing/Attributes.h"

namespace modules {

Testing::Testing(const Features& features)
    : api::Module(kTesting, "Testing")
    , enumeration_(new testing::Enumeration)
    , attributes_(new testing::Attributes) {
}

Testing::~Testing() {
  delete attributes_;
  delete enumeration_;
}

void Testing::ExtendObject(base::Object target) {
  enumeration_->AddTo(target);
  attributes_->AddTo(target);
}

void Testing::ExtendRuntime(api::Runtime& runtime) {
  AddTo(runtime.GetGlobalObject());
  ExtendObject(GetObject());
}

}
