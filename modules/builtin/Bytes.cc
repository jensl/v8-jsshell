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
#include "modules/builtin/Bytes.h"

#include <memory>
#include <algorithm>

#include "modules/BuiltIn.h"

namespace modules {
namespace builtin {

Bytes::Bytes()
    : Class("Bytes", &constructor) {
  AddGenericMethod<Bytes>("toString", &decode);
  AddGenericMethod<Bytes>("decode", &decode);
  AddGenericMethod<Bytes>("slice", &slice);
  AddGenericMethod<Bytes>("concat", &concat);
  AddGenericMethod<Bytes>("toJSON", &toJSON);
  AddClassFunction<Bytes>("encode", &encode);
  Inherit(base::Variant::MakeUint8Array(
      base::Variant::MakeArrayBuffer(0)).GetPrototype());
}

void* Bytes::Value::data() {
  return value_.ExtractArrayBufferData();
}

size_t Bytes::Value::length() {
  return value_.ExtractArrayBufferLength();
}

Bytes::Value Bytes::New(const std::string& data) {
  base::Variant result(base::Variant::MakeUint8Array(
      base::Variant::MakeArrayBuffer(data.c_str(), data.size())));
  result.AsObject().SetPrototype(GetPrototype());
  return result;
}

Bytes::Value Bytes::New(const void* data, size_t length) {
  base::Variant result(base::Variant::MakeUint8Array(
      base::Variant::MakeArrayBuffer(data, length)));
  result.AsObject().SetPrototype(GetPrototype());
  return result;
}

Bytes::Value Bytes::New(size_t length) {
  base::Variant result(base::Variant::MakeUint8Array(
      base::Variant::MakeArrayBuffer(length)));
  result.AsObject().SetPrototype(GetPrototype());
  return result;
}

Bytes* Bytes::FromContext(v8::Handle<v8::Context> context) {
  return BuiltIn::FromContext(context)->bytes();
}

Bytes::Value Bytes::constructor(
    Bytes* bytes, size_t length) {
  return bytes->New(length);
}

std::string Bytes::decode(Value instance, Optional<std::string> encoding) {
  if (!encoding.specified() ||
      encoding.value() == "ascii" ||
      encoding.value() == "utf-8") {
    return std::string(static_cast<const char*>(instance.data()),
                       instance.length());
  } else {
    throw base::TypeError("unsupported encoding");
  }
}

Bytes::Value Bytes::slice(Value instance, Optional<size_t> offset,
                          Optional<size_t> length) {
  size_t use_offset;

  if (offset.specified())
    use_offset = std::min(instance.length(), offset.value());
  else
    use_offset = 0;

  size_t use_length = instance.length() - use_offset;

  if (length.specified())
    use_length = std::min(use_length, length.value());

  return Bytes::FromContext()->New(
      static_cast<char*>(instance.data()) + use_offset, use_length);
}

Bytes::Value Bytes::concat(Value instance, Value other) {
  Value result(Bytes::FromContext()->New(instance.length() + other.length()));
  std::copy(static_cast<char*>(instance.data()),
            static_cast<char*>(instance.data()) + instance.length(),
            static_cast<char*>(result.data()));
  std::copy(static_cast<char*>(other.data()),
            static_cast<char*>(other.data()) + other.length(),
            static_cast<char*>(result.data()) + instance.length());
  return result;
}

std::string Bytes::toJSON(Value instance) {
  return std::string(static_cast<char*>(instance.data()), instance.length());
}

Bytes::Value Bytes::encode(Bytes* cls, std::string data,
                           Optional<std::string> encoding) {
  if (!encoding.specified() ||
      encoding.value() == "utf-8")
    return cls->New(data);
  else
    throw base::TypeError("unsupported encoding");
}

}
}

namespace conversions {

using namespace modules::builtin;

template <>
Bytes::Value as_value(const base::Variant& value, Bytes::Value*) {
  Bytes* bytes = Bytes::FromContext();
  if (!value.IsObject() ||
      !(value.IsArrayBuffer() || value.IsArrayBufferView()))
    return bytes->New(value.AsString());
  return Bytes::Value(value);
}

template <>
base::Variant as_result(Bytes::Value result) {
  return result;
}

}
