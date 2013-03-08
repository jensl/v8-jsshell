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

#ifndef MODULES_URL_HEADER_H
#define MODULES_URL_HEADER_H

#include "api/Class.h"

namespace modules {
namespace url {

class Header : public api::Class {
 public:
  class Instance;

  Header();

  Instance* New(std::string name, std::string value);

  static Header* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Header*, std::string name, std::string value);

  static std::string toString(Instance* instance);

  static std::string get_name(Instance* instance);
  static std::string get_value(Instance* instance);
};

}
}

namespace conversions {

using namespace modules::url;

template <>
Header::Instance* as_value(
    const base::Variant& value, Header::Instance**);

template <>
base::Variant as_result(Header::Instance* result);

}

#endif // MODULES_URL_HEADER_H
