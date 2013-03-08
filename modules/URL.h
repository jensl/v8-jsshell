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

#ifndef MODULES_URL_H
#define MODULES_URL_H

#include "api/Module.h"
#include "utilities/Options.h"

namespace modules {

class Features;

namespace url {
class Request;
class Header;
}

class URL : public api::Module {
 public:
  URL(const Features& features);
  ~URL();

  virtual void ExtendObject(base::Object target) override;
  virtual void ExtendRuntime(api::Runtime& runtime) override;

  url::Request* request() { return request_; }
  url::Header* header() { return header_; }

  static URL* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  url::Request* request_;
  url::Header* header_;

  static std::string get(URL*, std::string url, utilities::Options options);
  static std::string post(URL*, std::string url, std::string data,
                          utilities::Options options);
};

}

#endif // MODULES_URL_H
