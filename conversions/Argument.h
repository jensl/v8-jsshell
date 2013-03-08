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

#ifndef CONVERSIONS_ARGUMENT_H
#define CONVERSIONS_ARGUMENT_H

namespace conversions {

void check_argc(const std::vector<base::Variant>& argv, unsigned index);

template <typename R>
R as_argument(const std::vector<base::Variant>& argv, unsigned index,
              R* dummy) {
  check_argc(argv, index);
  return as_value(argv[index], dummy);
}

}

#endif // CONVERSIONS_ARGUMENT_H
