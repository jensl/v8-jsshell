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

namespace base {

class String {
public:
  String(std::string&& value)
      : value_(value) {
  }
  String(std::string value)
      : value_(value) {
  }

  v8::Local<v8::String> ToV8() const {
    return New(value_);
  }

  operator v8::Local<v8::String>() const {
    return ToV8();
  }

  static v8::Local<v8::String> New(std::string data) {
    return New(data.data(), data.size());
  }

  static v8::Local<v8::String> New(const char* data, int length = -1) {
    return v8::String::NewFromUtf8(
        CurrentIsolate(), data, v8::String::kNormalString, length);
  }

 private:
  std::string value_;
};

inline v8::Local<v8::Symbol> Symbol(std::string name) {
  return v8::Symbol::ForApi(CurrentIsolate(), String(name));
}

inline v8::Local<v8::Private> Private(std::string name) {
  return v8::Private::ForApi(CurrentIsolate(), String(name));
}

}

#endif // BASE_CONVERT_H

