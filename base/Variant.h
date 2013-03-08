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

#ifndef VARIANT_H
#define VARIANT_H

#include <cstdint>
#include <string>

#include "v8.h"

namespace base {

class Object;

class Variant {
 public:
  Variant();

  template <typename Value>
  Variant(Value value);

  template <typename Type>
  Variant(v8::Handle<Type> value);

  template <typename Type>
  Variant(v8::Local<Type> value);

  bool IsEmpty() const;

  bool IsUndefined() const;
  bool IsNull() const;
  bool IsBoolean() const;
  bool IsNumber() const;
  bool IsInteger() const;
  bool IsInt32() const;
  bool IsUInt32() const;
  bool IsString() const;
  bool IsObject() const;
  bool IsArrayObject() const;
  bool IsFunctionObject() const;
  bool IsBooleanObject() const;
  bool IsNumberObject() const;
  bool IsStringObject() const;
  bool IsDateObject() const;
  bool IsRegExpObject() const;
  bool IsExternal() const;

  bool AsBoolean() const;
  double AsNumber() const;
  std::int64_t AsInteger() const;
  std::int32_t AsInt32() const;
  std::uint32_t AsUInt32() const;
  std::string AsString() const;
  base::Object AsObject() const;
  std::string AsJSON() const;

  Variant ToBoolean() const;
  Variant ToNumber() const;
  Variant ToInteger() const;
  Variant ToInt32() const;
  Variant ToUInt32() const;
  Variant ToString() const;
  Variant ToObject() const;

  static Variant Undefined();
  static Variant Null();
  static Variant Boolean(bool value);
  static Variant Number(double value);
  static Variant Integer(std::int64_t value);
  static Variant Int32(std::int32_t value);
  static Variant UInt32(std::uint32_t value);
  static Variant String(std::string value);
  static Variant Object(base::Object value);

  void SetBoolean(bool value);
  void SetNumber(double value);
  void SetInteger(std::int64_t value);
  void SetInt32(std::int32_t value);
  void SetUInt32(std::uint32_t value);
  void SetString(std::string value);
  void SetObject(base::Object value);

  v8::Handle<v8::Value> handle() const { return handle_; }

 private:
  friend class Object;

  v8::Handle<v8::Value> handle_;
};

}

#endif // VARIANT_H
