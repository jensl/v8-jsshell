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

#ifndef CONVERSIONS_DEFAULT_H
#define CONVERSIONS_DEFAULT_H

namespace conversions {

template <typename R>
R as_value(const base::Variant& value, R*) {
  return R::no_value_conversion_implemented_for_this_type();
}

template <typename T>
base::Variant as_result(T result) {
  return T::no_result_conversion_implemented_for_this_type();
}

}

#endif // CONVERSIONS_H
