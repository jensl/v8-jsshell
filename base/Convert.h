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

#ifndef BASE_CONVERT_H
#define BASE_CONVERT_H

#include "conversions/Default.h"
#include "conversions/Fundamental.h"
#include "conversions/Array.h"
#include "conversions/Dictionary.h"
#include "conversions/Argument.h"
#include "conversions/Optional.h"

namespace base {

template <typename R>
R AsValue(const Variant& value) {
  return conversions::as_value(value, static_cast<R*>(NULL));
}

template <typename R>
R AsArgument(const std::vector<base::Variant>& args, unsigned index) {
  return conversions::as_argument(args, index, static_cast<R*>(NULL));
}

template <typename R>
Variant AsResult(R value) {
  return conversions::as_result(value);
}

}

#endif // BASE_CONVERT_H
