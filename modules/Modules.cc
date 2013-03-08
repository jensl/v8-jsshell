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

#include "modules/Modules.h"

namespace modules {

Features::Features(const std::map<std::string, bool>& features)
    : features_(features) {
}

bool Features::Enabled(std::string feature) const {
  auto iter(features_.find(feature));
  if (iter == features_.end())
    return false;
  else
    return iter->second;
}

bool Features::Disabled(std::string feature) const {
  auto iter(features_.find(feature));
  if (iter == features_.end())
    return false;
  else
    return !iter->second;
}

Features::const_iterator Features::begin() const {
  return features_.begin();
}

Features::const_iterator Features::end() const {
  return features_.end();
}

}
