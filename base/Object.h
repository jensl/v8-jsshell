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

#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "v8.h"

namespace base {

class Object {
 public:
  class Persistent {
   public:
    Persistent();
    Persistent(const Persistent& other);
    Persistent(const Object& object);
    ~Persistent();

    Persistent& operator= (const Persistent& other);
    Persistent& operator= (const Object& object);

    bool IsEmpty() const;
    Object GetObject() const;
    void Release();

   private:
    v8::Persistent<v8::Object> handle_;
  };

  Object();

  template <typename Type>
  Object(v8::Handle<Type> object);

  template <typename Type>
  Object(v8::Local<Type> object);

  static base::Object Create();

  bool IsEmpty();
  bool IsCallable();

  template <typename Class>
  typename Class::Instance* AsInstance(Class* cls) const;

  bool HasProperty(std::string name);
  bool HasProperty(std::uint32_t index);

  unsigned GetPropertyFlags(std::string name);
  unsigned GetPropertyFlags(std::uint32_t name);

  Variant Get(std::string name);
  Variant Get(std::uint32_t index);

  void Put(std::string name, const Variant& value,
           unsigned flags = PropertyFlags::kAll);
  void Put(std::uint32_t index, const Variant& value,
           unsigned flags = PropertyFlags::kAll);

  std::vector<std::string> GetPropertyNames();
  std::vector<std::string> GetOwnPropertyNames();

  bool HasHidden(std::string name);
  Variant GetHidden(std::string name);
  void PutHidden(std::string name, const Variant& value);

  Variant Call(Object this_object,
               const std::vector<Variant>& arguments = std::vector<Variant>());
  Variant Call(std::string method,
               const std::vector<Variant>& arguments = std::vector<Variant>());

  v8::Handle<v8::Object> handle() const { return handle_; }

  template <typename ValueType>
  static Object FromMap(const std::map<std::string, ValueType>& map,
                        unsigned flags = PropertyFlags::kAll);

  template <typename ValueType>
  static std::map<std::string, ValueType> ToMap(Object object);

 private:
  friend class Variant;
  friend class Persistent;

  v8::Handle<v8::Object> handle_;
};

template <typename Class>
typename Class::Instance* Object::AsInstance(Class* cls) const {
  if (cls->HasInstance(handle_)) {
    return static_cast<typename Class::Instance*>
        (handle_->GetAlignedPointerFromInternalField(0));
  } else {
    return NULL;
  }
}

}

#endif // BASE_OBJECT_H
