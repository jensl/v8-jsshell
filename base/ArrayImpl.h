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

#ifndef BASE_ARRAYIMPL_H
#define BASE_ARRAYIMPL_H

#include <cstdint>

#include "base/Array.h"
#include "base/Convert.h"

namespace base {

template <typename ValueType>
Object Array::FromVector(const std::vector<ValueType>& elements) {
  Object array(v8::Array::New(CurrentIsolate(), elements.size()));

  for (std::uint32_t index = 0; index < elements.size(); ++index)
    array.Put(index, AsResult(elements[index]));

  return array;
}

template <typename ValueType>
std::vector<ValueType> Array::ToVector(Object object) {
  std::vector<ValueType> result;

  if (object.HasProperty("length")) {
    std::uint32_t length = object.Get("length").AsUInt32();

    for (std::uint32_t index = 0; index < length; ++index)
      result.push_back(AsValue<ValueType>(object.Get(index).handle()));
  }

  return result;
}

}

#endif // BASE_ARRAYIMPL_H
