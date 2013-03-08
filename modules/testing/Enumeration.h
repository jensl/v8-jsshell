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

#ifndef MODULES_TESTING_ENUMERATION_H
#define MODULES_TESTING_ENUMERATION_H

#include "api/Class.h"

namespace modules {
namespace testing {

class Enumeration : public api::Class {
 public:
  class Instance;

  Enumeration();

 private:
  static Instance* constructor(Enumeration*);

  static std::vector<std::string> list(Instance* instance);
  static unsigned query(Instance* instance, std::string name);
  static base::Variant get(Instance* instance, std::string name);
};

}
}

#endif // MODULES_TESTING_ENUMERATION_H
