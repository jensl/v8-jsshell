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

namespace conversions {

template <>
bool as_value(const base::Variant& value, bool*) {
  return value.AsBoolean();
}

template <>
int as_value(const base::Variant& value, int*) {
  return value.AsInt32();
}

template <>
unsigned as_value(const base::Variant& value, unsigned*) {
  return value.AsUInt32();
}

template <>
std::int64_t as_value(const base::Variant& value, std::int64_t*) {
  return value.AsInteger();
}

template <>
std::size_t as_value(const base::Variant& value, std::size_t*) {
  return value.AsInteger();
}

template <>
double as_value(const base::Variant& value, double*) {
  return value.AsNumber();
}

template <>
std::string as_value(const base::Variant& value, std::string*) {
  return value.AsString();
}

template <>
base::Variant as_value(const base::Variant& value, base::Variant*) {
  return value;
}

template <>
base::Object as_value(const base::Variant& value, base::Object*) {
  return value.AsObject();
}

template <>
base::Function as_value(const base::Variant& value, base::Function*) {
  base::Object object(value.AsObject());
  return base::Function(object);
}

template <>
base::Variant as_result(bool result) {
  return base::Variant::Boolean(result);
}

template <>
base::Variant as_result(int result) {
  return base::Variant::Int32(result);
}

template <>
base::Variant as_result(unsigned result) {
  return base::Variant::UInt32(result);
}

template <>
base::Variant as_result(std::int64_t result) {
  return base::Variant::Integer(result);
}

template <>
base::Variant as_result(double result) {
  return base::Variant::Number(result);
}

template <>
base::Variant as_result(std::string result) {
  return base::Variant::String(result);
}

template <>
base::Variant as_result(base::Variant result) {
  return result;
}

template <>
base::Variant as_result(base::Object result) {
  return base::Variant::Object(result);
}

template <>
base::Variant as_result(base::Function result) {
  return base::Variant::Object(result.object());
}

}
