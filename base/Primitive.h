/* -*- mode: c++ -*- */
/*

  Copyright 2016 Jens Widell

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

#ifndef BASE_PRIMITIVE_H
#define BASE_PRIMITIVE_H

#include "v8.h"

namespace base {

class Primitive {
public:
  static v8::Local<v8::Integer> New(int32_t value) {
    return v8::Integer::New(v8::Isolate::GetCurrent(), value);
  }

  static v8::Local<v8::Integer> New(uint32_t value) {
    return v8::Integer::NewFromUnsigned(v8::Isolate::GetCurrent(), value);
  }
};

}

#endif // BASE_CONVERT_H

