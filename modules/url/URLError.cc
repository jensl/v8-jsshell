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

#if LIBCURL_SUPPORT

#include "Base.h"
#include "modules/url/URLError.h"

#include <string.h>
#include <stdio.h>

#include <memory>

namespace modules {
namespace url {

URLError::URLError(std::string message)
    : base::CustomError("URLError") {
  set_message(message);
}

URLError::URLError(std::string prefix, CURLcode error)
    : base::CustomError("URLError") {
  set_message(prefix + ": " + curl_easy_strerror(error));
}

URLError& URLError::operator<< (const std::string& string) {
  set_message(message() + string);
  return *this;
}

}
}

#endif
