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

#ifndef MODULES_BUILTIN_BYTES_H
#define MODULES_BUILTIN_BYTES_H

#include "api/Class.h"

namespace modules {
namespace builtin {

class Module;

class Bytes : public api::Class {
 public:
  class Instance;

  Bytes();

  static const std::string& data(Instance* instance);

  Bytes::Instance* New(const std::string& data);

  static Bytes* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Bytes*, unsigned length);

  static std::string decode(Instance* instance, Optional<std::string> encoding);

  static Instance* slice(Instance* instance, Optional<unsigned> offset,
                         Optional<unsigned> length);
  static Instance* concat(Instance* instance, Instance* other);

  static std::string toJSON(Instance* instance);

  static Instance* encode(Bytes* cls, std::string data,
                          Optional<std::string> encoding);

  static unsigned get_length(Bytes::Instance* instance);
};

}
}

namespace conversions {

using namespace modules::builtin;

template <>
Bytes::Instance* as_value(
    const base::Variant& value, Bytes::Instance**);

template <>
base::Variant as_result(Bytes::Instance* result);

}

#endif // MODULES_BUILTIN_BYTES_H
