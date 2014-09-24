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

#ifndef CONVERSIONS_FUNDAMENTAL_H
#define CONVERSIONS_FUNDAMENTAL_H

#include <string>

#include "conversions/Default.h"

namespace base {
class Object;
class Array;
}

namespace conversions {

template <>
bool as_value(const base::Variant& value, bool*);

template <>
int as_value(const base::Variant& value, int*);

template <>
unsigned as_value(const base::Variant& value, unsigned*);

template <>
std::int64_t as_value(const base::Variant& value, std::int64_t*);

template <>
double as_value(const base::Variant& value, double*);

template <>
std::string as_value(const base::Variant& value, std::string*);

template <>
base::Variant as_value(const base::Variant& value, base::Variant*);

template <>
base::Object as_value(const base::Variant& value, base::Object*);

template <>
base::Array as_value(const base::Variant& value, base::Array*);

template <>
base::Function as_value(const base::Variant& value, base::Function*);

template <>
base::Variant as_result(bool result);

template <>
base::Variant as_result(int result);

template <>
base::Variant as_result(unsigned result);

template <>
base::Variant as_result(std::int64_t result);

template <>
base::Variant as_result(double result);

template <>
base::Variant as_result(std::string result);

template <>
base::Variant as_result(base::Variant);

template <>
base::Variant as_result(base::Object);

template <>
base::Variant as_result(base::Array);

template <>
base::Variant as_result(base::Function);

}

#endif // CONVERSIONS_H
