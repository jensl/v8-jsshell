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
#include "api/Class.h"
#include "api/Module.h"

namespace api {

Class::~Class() {
  template_.Reset();
}

bool Class::HasInstance(base::Object object) {
  return function_template()->HasInstance(object.handle());
}

void Class::AddTo(base::Object target) {
  target.Put(name_, GetConstructor());
}

void Class::AddTo(Module& module) {
  AddTo(module.GetObject());
}

base::Object Class::NewInstance() {
  return function_template()->InstanceTemplate()->NewInstance();
}

base::Object Class::NewInstance(
    const std::vector<base::Variant>& argv) {
  return base::Function(GetConstructor()).Construct(argv);
}

base::Object Class::GetConstructor() {
  return base::Object(function_template()->GetFunction());
}

base::Object Class::GetPrototype() {
  base::Variant prototype_value(GetConstructor().Get("prototype"));
  return prototype_value.AsObject();
}

Class::Class(std::string name)
    : context_(CurrentIsolate(),
               CurrentIsolate()->GetEnteredContext())
    , template_(CurrentIsolate(), v8::FunctionTemplate::New(CurrentIsolate()))
    , name_(name) {
  function_template()->SetClassName(base::String::New(name));
  function_template()->InstanceTemplate()->SetInternalFieldCount(1);
}

Class::Class(std::string name, const glue::ConstructorGlue& constructor)
    : Class(name) {
  constructor.SetCallHandler(function_template());
}

Class::Class(std::string name, const glue::GenericConstructorGlue& constructor)
    : Class(name) {
  constructor.SetCallHandler(function_template());
}

void Class::Inherit(Class* parent) {
  function_template()->Inherit(parent->function_template());
}

void Class::Inherit(base::Object parent) {
  GetPrototype().SetPrototype(parent);
}

}
