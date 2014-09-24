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
  class Value {
   public:
    Value() {
    }

    Value(base::Variant value)
        : value_(value) {
    }

    void* data();
    size_t length();

    operator std::string() {
      return std::string(static_cast<char*>(data()), length());
    }

    operator base::Variant() {
      return value_;
    }

   private:
    base::Variant value_;
  };

  Bytes();

  Value New(const std::string& data);
  Value New(const void* data, size_t length);
  Value New(size_t length);

  static Bytes* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Value constructor(Bytes*, size_t length);

  static std::string decode(Value, Optional<std::string> encoding);

  static Value slice(Value, Optional<size_t> offset, Optional<size_t> length);
  static Value concat(Value instance, Value other);

  static std::string toJSON(Value);

  static Value encode(Bytes* cls, std::string data,
                      Optional<std::string> encoding);
};

}
}

namespace conversions {

using namespace modules::builtin;

template <>
Bytes::Value as_value(
    const base::Variant& value, Bytes::Value*);

template <>
base::Variant as_result(Bytes::Value result);

}

#endif // MODULES_BUILTIN_BYTES_H
