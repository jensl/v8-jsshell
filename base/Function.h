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

#ifndef BASE_FUNCTION_H
#define BASE_FUNCTION_H

namespace base {

class Function {
 public:
  Function();
  Function(Object object);

  Variant Call(Object this_object,
               const std::vector<Variant>& arguments = std::vector<Variant>());
  Object Construct(
      const std::vector<Variant>& arguments = std::vector<Variant>());

  Object object() { return object_; }

 private:
  Object object_;
};

}

#endif // BASE_OBJECT_H
