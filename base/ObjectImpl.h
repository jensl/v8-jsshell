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

#ifndef BASE_OBJECTIMPL_H
#define BASE_OBJECTIMPL_H

#include <cstdint>

#include "base/Array.h"
#include "base/Convert.h"

namespace base {

template <typename Type>
Object::Object(v8::Handle<Type> handle)
    : handle_(handle) {
}

template <typename Type>
Object::Object(const v8::Persistent<Type>& handle)
    : handle_(v8::Local<v8::Object>::New(CurrentIsolate(), handle)) {
}

template <typename ValueType>
Object Object::FromMap(const std::map<std::string, ValueType>& map,
                      unsigned flags) {
  Object object(v8::Object::New(CurrentIsolate()));

  typename std::map<std::string, ValueType>::const_iterator iter(map.begin());

  while (iter != map.end()) {
    object.Put(iter->first, AsResult(iter->second));
    ++iter;
  }

  return object;
}

template <typename ValueType>
std::map<std::string, ValueType> Object::ToMap(Object object) {
  std::map<std::string, ValueType> map;

  v8::Local<v8::Array> names(object.handle()->GetPropertyNames());
  std::uint32_t length = names->Length();

  for (std::uint32_t index = 0; index < length; ++index) {
    std::string name(Variant(names->Get(index)).AsString());
    map[name] = AsValue<ValueType>(object.Get(name).handle());
  }

  return map;
}

}

#endif // BASE_OBJECTIMPL_H
