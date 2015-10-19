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

#ifndef MODULES_URL_URLERROR_H
#define MODULES_URL_URLERROR_H

#include "modules/url/Request.h"

#include <curl/curl.h>

namespace modules {
namespace url {

class URLError : public base::CustomError {
 public:
  URLError(std::string message);
  URLError(std::string message, base::Variant request);
  URLError(std::string prefix, CURLcode error);

  URLError& operator<< (const std::string& string);

 protected:
  base::Object Create() override;

 private:
  base::Variant request_;
};

}
}

#endif // MODULES_URL_URLERROR_H
