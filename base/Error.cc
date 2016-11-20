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

#include "Base.h"

#include "v8.h"

namespace base {

Error::Error()
    : message_("<no message>") {
}

Error::Error(std::string message)
    : message_(message) {
}

TypeError::TypeError(std::string message)
    : Error(message) {
}

TypeError& TypeError::operator<< (const std::string& string) {
  set_message(message() + string);
  return *this;
}

void TypeError::Raise() {
  CurrentIsolate()->ThrowException(v8::Exception::TypeError(
      base::String::New(message().c_str(), message().length())));
}

ReferenceError::ReferenceError(std::string message)
    : Error(message) {
}

ReferenceError& ReferenceError::operator<< (const std::string& string) {
  set_message(message() + string);
  return *this;
}

void ReferenceError::Raise() {
  CurrentIsolate()->ThrowException(v8::Exception::ReferenceError(
      base::String::New(message().c_str(), message().length())));
}

SyntaxError::SyntaxError(std::string message)
    : Error(message) {
}

SyntaxError& SyntaxError::operator<< (const std::string& string) {
  set_message(message() + string);
  return *this;
}

void SyntaxError::Raise() {
  CurrentIsolate()->ThrowException(v8::Exception::SyntaxError(
      base::String::New(message().c_str(), message().length())));
}

RangeError::RangeError(std::string message)
    : Error(message) {
}

RangeError& RangeError::operator<< (const std::string& string) {
  set_message(message() + string);
  return *this;
}

void RangeError::Raise() {
  CurrentIsolate()->ThrowException(v8::Exception::RangeError(
      base::String::New(message().c_str(), message().length())));
}

CustomError::CustomError(std::string name)
    : name_(name) {
}

CustomError::CustomError(std::string name, std::string message)
    : Error(message)
    , name_(name) {
}

void CustomError::Raise() {
  CurrentIsolate()->ThrowException(Create());
}

v8::Local<v8::Object> CustomError::Create() {
  v8::Local<v8::Object> error(v8::Exception::Error(
      base::String::New(message().c_str(), message().length()))->ToObject());
  error->Set(base::String::New("name"),
             base::String::New(name_.c_str(), name_.length()));
  return error;
}

UncaughtException::UncaughtException(v8::TryCatch& try_catch)
    : exception_(try_catch.Exception())
    , message_(try_catch.Message()) {
  try_catch.Reset();
}

FileNotFound::FileNotFound(std::string filename)
    : filename_(filename) {
}

}
