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

#ifndef ERROR_H
#define ERROR_H

#include <string>

#include "v8.h"

namespace base {

class Error {
 public:
  Error();
  explicit Error(std::string message);

  virtual void Raise() = 0;

  std::string message() { return message_; }
  void set_message(std::string message) { message_ = message; }

 private:
  std::string message_;
};

class TypeError : public Error {
 public:
  TypeError(std::string message);

  TypeError& operator<< (const std::string& string);

  virtual void Raise();
};

class ReferenceError : public Error {
 public:
  ReferenceError(std::string message);

  ReferenceError& operator<< (const std::string& string);

  virtual void Raise();
};

class SyntaxError : public Error {
 public:
  SyntaxError(std::string message);

  SyntaxError& operator<< (const std::string& string);

  virtual void Raise();
};

class RangeError : public Error {
 public:
  RangeError(std::string message);

  RangeError& operator<< (const std::string& string);

  virtual void Raise();
};

class CustomError : public Error {
 protected:
  explicit CustomError(std::string name);
  CustomError(std::string name, std::string message);

  virtual void Raise();

  v8::Local<v8::Object> Create();

 private:
  std::string name_;
};

class UncaughtException {
 public:
  UncaughtException(v8::TryCatch& try_catch);

  v8::Local<v8::Value> exception() { return exception_; }
  v8::Local<v8::Message> message() { return message_; }

 private:
  v8::Local<v8::Value> exception_;
  v8::Local<v8::Message> message_;
};

/* Thrown when a nested call to the engine results in an exception being thrown.
   By default, this simply aborts the current call or property access and
   returns back to the engine, which propagates the exception. */
class NestedException {};

class FileNotFound {
 public:
  FileNotFound(std::string filename);

  const std::string& filename() const { return filename_; }

 private:
  std::string filename_;
};

}

#endif // ERROR_H
