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
#include "modules/testing/Attributes.h"

namespace modules {
namespace testing {

class Attributes::Instance : public api::Class::Instance<Attributes> {
 public:
};

Attributes::Attributes()
    : api::Class("Attributes", constructor) {
  AddNamedHandler<Attributes>(list, query, get, set);
}

Attributes::Instance* Attributes::constructor(Attributes*) {
  return new Instance;
}

std::vector<std::string> Attributes::list(Instance* instance) {
  return { "[]", "[w]", "[e]", "[c]", "[we]", "[wc]", "[ec]", "[wec]" };
}

unsigned Attributes::query(Instance* instance, std::string name) {
  unsigned flags = 0;
  if (name.find("w") != std::string::npos)
    flags |= base::PropertyFlags::kWritable;
  if (name.find("e") != std::string::npos)
    flags |= base::PropertyFlags::kEnumerable;
  if (name.find("c") != std::string::npos)
    flags |= base::PropertyFlags::kConfigurable;
  return flags;
}

bool Attributes::get(Instance* instance, std::string name) {
  return true;
}

void Attributes::set(Instance* instance, std::string name, bool value) {
}

}
}
