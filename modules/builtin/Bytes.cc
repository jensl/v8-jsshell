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

class Bytes::Instance : public Class::Instance<Bytes> {
 public:
  Instance();
  Instance(std::string data);
  ~Instance();

  void InitializeObject();

  std::string data;
};

Bytes::Instance::Instance() {
}

Bytes::Instance::Instance(std::string data)
    : data(data) {
  v8::Isolate::GetCurrent()
      ->AdjustAmountOfExternalAllocatedMemory(data.length());
}

Bytes::Instance::~Instance() {
  int64_t length = -static_cast<intptr_t>(data.length());
  v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(length);
}

void Bytes::Instance::InitializeObject() {
  GetObject().handle()->SetIndexedPropertiesToExternalArrayData(
      const_cast<char*>(data.c_str()), v8::kExternalUnsignedByteArray,
      data.length());
}

Bytes::Bytes()
    : Class("Bytes", &constructor) {
  AddMethod<Bytes>("toString", &decode);
  AddMethod<Bytes>("decode", &decode);
  AddMethod<Bytes>("slice", &slice);
  AddMethod<Bytes>("concat", &concat);
  AddMethod<Bytes>("toJSON", &toJSON);
  AddProperty<Bytes>("length", &get_length);
  AddClassFunction<Bytes>("encode", &encode);
}

const std::string& Bytes::data(Instance* instance) {
  return instance->data;
}

Bytes::Instance* Bytes::New(const std::string& data) {
  Instance* instance = new Instance(data);
  instance->CreateObject(this);
  return instance;
}

Bytes* Bytes::FromContext(v8::Handle<v8::Context> context) {
  return BuiltIn::FromContext(context)->bytes();
}

Bytes::Instance* Bytes::constructor(
    Bytes*, unsigned length) {
  return new Instance(std::string(length, '\0'));
}

std::string Bytes::decode(Instance* bytes, Optional<std::string> encoding) {
  if (!encoding.specified() ||
      encoding.value() == "ascii" ||
      encoding.value() == "utf-8")
    return bytes->data;
  else
    throw base::TypeError("unsupported encoding");
}

Bytes::Instance* Bytes::slice(Instance* instance, Optional<unsigned> offset,
                              Optional<unsigned> length) {
  unsigned use_offset;

  if (offset.specified())
    use_offset = std::min(static_cast<unsigned>(instance->data.length()),
                          offset.value());
  else
    use_offset = 0;

  unsigned use_length = instance->data.length() - use_offset;

  if (length.specified())
    use_length = std::min(use_length, length.value());

  return instance->GetClass()->New(
      std::string(instance->data, use_offset, use_length));
}

Bytes::Instance* Bytes::concat(Instance* instance, Instance* other) {
  return instance->GetClass()->New(instance->data + other->data);
}

std::string Bytes::toJSON(Instance* instance) {
  return instance->data;
}

Bytes::Instance* Bytes::encode(Bytes* cls, std::string data,
                               Optional<std::string> encoding) {
  if (!encoding.specified() ||
      encoding.value() == "utf-8")
    return cls->New(data);
  else
    throw base::TypeError("unsupported encoding");
}

unsigned Bytes::get_length(Bytes::Instance* instance) {
  return instance->data.length();
}

}
}

namespace conversions {

using namespace modules::builtin;

template <>
Bytes::Instance* as_value(
    const base::Variant& value, Bytes::Instance**) {
  Bytes* bytes = Bytes::FromContext();
  if (!value.IsObject() || !bytes->HasInstance(value.AsObject()))
    return bytes->New(value.AsString());
  return Bytes::Instance::FromObject(bytes, value.AsObject());
}

template <>
base::Variant as_result(Bytes::Instance* result) {
  if (!result)
    return base::Variant::Null();
  return result->GetObject();
}

}
