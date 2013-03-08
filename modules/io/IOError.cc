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
#include "modules/io/IOError.h"

#include <string.h>
#include <stdio.h>

#include <memory>

namespace modules {
namespace io {

IOError::IOError(std::string message)
    : base::CustomError("IOError") {
  set_message(message);
}

IOError::IOError(std::string prefix, int errnum)
    : base::CustomError("IOError") {
  const char *error = strerror(errnum);
  std::unique_ptr<char[]> buffer(new char[prefix.length() + strlen(error) + 3]);
  sprintf(buffer.get(), "%s: %s", prefix.c_str(), error);
  set_message(std::string(buffer.get()));
}

IOError& IOError::operator<< (const std::string& string) {
  set_message(message() + string);
  return *this;
}

}
}
