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
#include "modules/URL.h"
#include "modules/url/Header.h"

namespace modules {
namespace url {

class Header::Instance : public api::Class::Instance<Header> {
 public:
  Instance(std::string name, std::string value)
      : name(name)
      , value(value) {
  }

  std::string name;
  std::string value;
};

Header::Header()
    : api::Class("Header", &constructor) {
  AddMethod<Header>("toString", &toString);

  AddProperty<Header>("name", &get_name);
  AddProperty<Header>("value", &get_value);
}

Header::Instance* Header::New(std::string name, std::string value) {
  Instance* instance = new Instance(name, value);
  instance->CreateObject(this);
  return instance;
}

Header* Header::FromContext(v8::Handle<v8::Context> context) {
  return URL::FromContext(context)->header();
}

Header::Instance* Header::constructor(Header*, std::string name,
                                        std::string value) {
  return new Instance(name, value);
}

std::string Header::toString(Instance* instance) {
  return instance->name + ": " + instance->value;
}

std::string Header::get_name(Instance* instance) {
  return instance->name;
}

std::string Header::get_value(Instance* instance) {
  return instance->value;
}

}
}

namespace conversions {

using namespace modules::url;

template <>
Header::Instance* as_value(
    const base::Variant& value, Header::Instance**) {
  Header* header = Header::FromContext();
  if (!value.IsObject() || !header->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected Header object");
  return Header::Instance::FromObject(header, value.AsObject());
}

template <>
base::Variant as_result(Header::Instance* result) {
  if (!result)
    return v8::Null();
  return result->GetObject().handle();
}

}
