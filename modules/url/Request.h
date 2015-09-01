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

#ifndef MODULES_URL_REQUEST_H
#define MODULES_URL_REQUEST_H

#include "api/Class.h"
#include "modules/builtin/Bytes.h"
#include "utilities/Options.h"

namespace modules {
namespace url {

class Request : public api::Class {
 public:
  class Instance;

  Request();

  static std::string get(std::string url, const utilities::Options& options);
  static std::string post(std::string url, std::string data,
                          const utilities::Options& options);
  static std::string put(std::string url, std::string data,
                          const utilities::Options& options);
  static std::string doOperation(std::string url, std::string method, std::string data,
                                 const utilities::Options& options);

  static Request* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Request*, std::string method, std::string url);

  static void setCredentials(Instance* instance, std::string username,
                             std::string password);
  static void setRequestHeader(Instance* instance, std::string name,
                               std::string value);
  static void setRequestBody(Instance* instance,
                             builtin::Bytes::Value value);
  static void perform(Instance* instance);

  static std::string get_statusLine(Instance* instance);
  static base::Object get_responseHeaders(Instance* instance);
  static builtin::Bytes::Value get_responseBody(Instance* instance);

  static void HandleOptions(Request::Instance* instance,
                            const utilities::Options& options);
};

}
}

#endif // MODULES_URL_REQUEST_H
