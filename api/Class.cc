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
  template_.Dispose(v8::Isolate::GetCurrent());
}

bool Class::HasInstance(base::Object object) {
  return template_->HasInstance(object.handle());
}

void Class::AddTo(base::Object target) {
  target.Put(name_, template_->GetFunction());
}

void Class::AddTo(Module& module) {
  AddTo(module.GetObject());
}

base::Object Class::NewInstance() {
  return template_->InstanceTemplate()->NewInstance();
}

base::Object Class::NewInstance(
    const std::vector<base::Variant>& argv_in) {
  std::vector<v8::Handle<v8::Value>> argv_out;
  std::vector<base::Variant>::const_iterator iter(argv_in.begin());
  while (iter != argv_in.end())
    argv_out.push_back(iter++->handle());
  return template_->GetFunction()->NewInstance(
      argv_out.size(), argv_out.data());
}

Class::Class(std::string name, const glue::ConstructorGlue& constructor)
    : context_(v8::Persistent<v8::Context>::New(v8::Isolate::GetCurrent(),
                                                v8::Context::GetEntered()))
    , template_(v8::Persistent<v8::FunctionTemplate>::New(
        v8::Isolate::GetCurrent(), v8::FunctionTemplate::New()))
    , name_(name) {
  template_->SetClassName(v8::String::New(name.c_str(), name.length()));
  template_->InstanceTemplate()->SetInternalFieldCount(1);

  constructor.SetCallHandler(template_);
}

void Class::Inherit(Class* parent) {
  template_->Inherit(parent->template_);
}

}
