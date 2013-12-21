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

#ifndef BASE_STRING_H
#define BASE_STRING_H

#include "v8.h"

namespace base {

class String {
public:
  static v8::Local<v8::String> New(const char* data, int length = -1) {
    return v8::String::NewFromUtf8(
        v8::Isolate::GetCurrent(), data, v8::String::kNormalString, length);
  }
};

}

#endif // BASE_CONVERT_H

