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

namespace base {

Function::Function(base::Object object)
    : object_(object) {
  if (!object.IsCallable())
    throw base::TypeError("Object is not callable");
}

Variant Function::Call(Object this_object_in,
                       const std::vector<Variant>& arguments_in) {
  v8::EscapableHandleScope handle_scope(v8::Isolate::GetCurrent());
  v8::Local<v8::Object> this_object;

  if (this_object_in.IsEmpty())
    this_object = v8::Isolate::GetCurrent()->GetCurrentContext()->Global();
  else
    this_object = this_object_in.handle();

  v8::Local<v8::Function> function(object_.handle().As<v8::Function>());

  std::vector<v8::Handle<v8::Value>> arguments;

  for (std::vector<Variant>::const_iterator iter = arguments_in.begin();
       iter != arguments_in.end();
       ++iter)
    arguments.push_back(iter->handle());

  v8::Local<v8::Value> returned(
      function->Call(this_object, arguments.size(), arguments.data()));

  if (returned.IsEmpty())
    throw NestedException();

  return handle_scope.Escape(returned);
}

}
