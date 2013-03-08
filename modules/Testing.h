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

#ifndef MODULES_TESTING_H
#define MODULES_TESTING_H

#include "api/Module.h"
#include "utilities/Options.h"

namespace modules {

class Features;

namespace testing {
class Enumeration;
class Attributes;
}

class Testing : public api::Module {
 public:
  Testing(const Features& features);
  ~Testing();

  virtual void ExtendObject(base::Object target) override;
  virtual void ExtendRuntime(api::Runtime& runtime) override;

 private:
  testing::Enumeration* enumeration_;
  testing::Attributes* attributes_;
};

}

#endif // MODULES_TESTING_H
