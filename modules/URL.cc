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
#include "modules/URL.h"

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/url/Request.h"
#include "modules/url/Header.h"

namespace modules {

URL::URL(const Features& features)
    : api::Module(kURL, "URL")
    , request_(new url::Request)
    , header_(new url::Header) {
}

URL::~URL() {
  delete request_;
  delete header_;
}

void URL::ExtendObject(base::Object target) {
  AddFunction(target, "get", get);
  AddFunction(target, "post", post);
  AddFunction(target, "put", put);

  request_->AddTo(target);
  header_->AddTo(target);
}

void URL::ExtendRuntime(api::Runtime& runtime) {
  AddTo(runtime.GetGlobalObject());
  ExtendObject(GetObject());
}

URL* URL::FromContext(v8::Handle<v8::Context> context) {
  return api::Module::FromContext<URL>(kURL, context);
}

std::string URL::get(URL*, std::string url, utilities::Options options) {
  return url::Request::get(url, options);
}

std::string URL::post(URL*, std::string url, std::string data,
                     utilities::Options options) {
  return url::Request::post(url, data, options);
}

std::string URL::put(URL*, std::string url, std::string data,
                     utilities::Options options) {
  return url::Request::put(url, data, options);
}

}

#endif // LIBCURL_SUPPORT
