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

#ifndef CONVERSIONS_DICTIONARY_H
#define CONVERSIONS_DICTIONARY_H

#include <map>
#include <string>

namespace conversions {

template <typename R>
std::map<std::string, R> as_value(const base::Variant& value,
                                  std::map<std::string, R>*) {
  return base::Object::ToMap<R>(as_value(
      value, static_cast<base::Object*>(NULL)));
}

template <typename T>
base::Variant as_result(std::map<std::string, T> result) {
  return base::Object::FromMap(result);
}

}

#endif // CONVERSIONS_DICTIONARY_H
