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
#include "modules/url/Request.h"

#include <curl/curl.h>
#include <stdexcept>
#include <string.h>

#include "modules/URL.h"
#include "modules/url/Header.h"
#include "modules/url/URLError.h"
#include "utilities/Anchor.h"

namespace modules {
namespace url {

class Request::Instance : public api::Class::Instance<Request> {
 public:
  Instance(std::string method, std::string url)
      : method(method)
      , url(url)
      , auth_method(CURLAUTH_NONE)
      , verify_peer(true)
      , performed(false)
      , status_code(0) {
  }

  std::string method;
  std::string url;
  std::vector<std::pair<std::string, std::string>> request_headers;
  std::string username;
  std::string password;
  long auth_method;
  bool verify_peer;
  std::string request_body;
  size_t request_body_sent;
  bool performed;
  std::string status_line;
  int status_code;
  std::vector<std::pair<std::string, std::string>> response_headers;
  std::string response_body;
};

namespace {

CURL *curl_handle = NULL;

void
Initialize()
{
  CURLcode error = curl_global_init(CURL_GLOBAL_ALL);
  if (error != 0)
    throw URLError("curl_global_init() failed", error);

  curl_handle = curl_easy_init();
  if (!curl_handle)
    throw URLError("curl_easy_init() failed");
}

extern "C" size_t
HeaderFunction(void* buffer, size_t size, size_t nmemb, void* data) {
  Request::Instance* instance = static_cast<Request::Instance*>(data);
  std::string line(static_cast<char*>(buffer), size * nmemb - 2);

  if (line.compare(0, 5, "HTTP/") == 0) {
    instance->status_line = line;
    instance->response_headers.clear();

    std::string::size_type space1 = line.find(" ");
    std::string::size_type space2 = line.find(" ", space1 + 1);
    if (space1 != std::string::npos && space2 != std::string::npos) {
      std::string status_code_str(line, space1 + 1, space2 - space1 - 1);
      try {
        instance->status_code = std::stoi(status_code_str);
      } catch (const std::logic_error& error) {
        // ignore possible invalid_argument and out_of_range exception
      }
    }
  } else if (line.length()) {
    std::string::size_type colon = line.find(":");
    std::string::size_type start(colon + 1);

    while (start < line.length() && (line[start] == ' ' || line[start] == '\t'))
      ++start;

    std::string name(line, 0, colon);
    std::string value(line, start, std::string::npos);

    instance->response_headers.push_back(std::make_pair(name, value));
  }

  return size * nmemb;
}

extern "C" size_t
WriteFunction(void* buffer, size_t size, size_t nmemb, void* data) {
  Request::Instance* instance = static_cast<Request::Instance*>(data);

  instance->response_body +=
      std::string(static_cast<char*>(buffer), size * nmemb);

  return size * nmemb;
}

extern "C" size_t
ReadFunction(void* buffer, size_t size, size_t nmemb, void* data) {
  Request::Instance* instance = static_cast<Request::Instance*>(data);
  size_t to_send = std::min(
      size * nmemb,
      instance->request_body.length() - instance->request_body_sent);

  memcpy(buffer,
         instance->request_body.c_str() + instance->request_body_sent, to_send);
  instance->request_body_sent += to_send;
  return to_send;
}

}

Request::Request()
    : api::Class("Request", &constructor) {
  AddMethod<Request>("setCredentials", &setCredentials);
  AddMethod<Request>("setRequestHeader", &setRequestHeader);
  AddMethod<Request>("setRequestBody", &setRequestBody);
  AddMethod<Request>("setVerifyPeer", &setVerifyPeer);
  AddMethod<Request>("perform", &perform);

  AddProperty<Request>("statusLine", &get_statusLine);
  AddProperty<Request>("statusCode", &get_statusCode);
  AddProperty<Request>("responseHeaders", &get_responseHeaders);
  AddProperty<Request>("responseBody", &get_responseBody);
}

Request::Instance* Request::New(std::string method, std::string url) {
    Instance* instance = new Instance(method, url);
    instance->CreateObject(this);
    return instance;
}

std::string Request::doOperation(std::string method, std::string url,
                                 std::string data,
                                 const utilities::Options& options) {
  Request::Instance* instance = Request::FromContext()->New(method, url);

  HandleOptions(instance, options);

  if (!data.empty())
    setRequestBody(instance, builtin::Bytes::FromContext()->New(data));
  perform(instance);

  if (instance->status_code / 100 != 2)
    throw URLError("request failed", instance);

  return instance->response_body;
}

std::string Request::get(std::string url, const utilities::Options& options) {
  return doOperation("GET", url, "", options);
}

std::string Request::post(std::string url, std::string data,
                          const utilities::Options& options) {
  return doOperation("POST", url, data, options);
}

std::string Request::put(std::string url, std::string data,
                          const utilities::Options& options) {
  return doOperation("PUT", url, data, options);
}

std::string Request::del(std::string url, const utilities::Options& options) {
  return doOperation("DELETE", url, "", options);
}

Request* Request::FromContext(v8::Handle<v8::Context> context) {
  return URL::FromContext(context)->request();
}

Request::Instance* Request::constructor(Request*, std::string method,
                                        std::string url) {
  return new Instance(method, url);
}

void Request::setCredentials(Instance* instance, std::string username,
                             std::string password,
                             Optional<std::string> method) {
  instance->username = username;
  instance->password = password;

  if (method.specified()) {
    if (method.value() == "basic")
      instance->auth_method = CURLAUTH_BASIC;
    else if (method.value() == "digest")
      instance->auth_method = CURLAUTH_DIGEST;
    else if (method.value() == "any")
      instance->auth_method = CURLAUTH_ANY;
    else
      throw URLError("invalid authentication method: ") << method.value();
  } else {
    instance->auth_method = CURLAUTH_ANY;
  }
}

void Request::setRequestHeader(Instance* instance, std::string name,
                               std::string value) {
  instance->request_headers.push_back(std::make_pair(name, value));
}

void Request::setRequestBody(Instance* instance,
                             builtin::Bytes::Value value) {
  if (instance->method != "POST" && instance->method != "PUT")
    throw URLError("invalid call to setRequestBody(); "
                   "method must be \"POST\" or \"PUT\"");

  instance->request_body = value;
  instance->request_body_sent = 0;
}

void Request::setVerifyPeer(Instance* instance, bool value) {
  if (instance->performed)
    throw URLError("request already performed");

  instance->verify_peer = value;
}

void Request::perform(Instance* instance) {
  if (instance->performed)
    throw URLError("request already performed");

  if (!curl_handle)
    Initialize();
  else
    curl_easy_reset(curl_handle);

  struct curl_slist *headers = NULL;

  if (instance->request_headers.size()) {
    auto iter(instance->request_headers.begin());

    while (iter != instance->request_headers.end()) {
      std::string header = iter->first + ": " + iter->second;
      headers = curl_slist_append(headers, header.c_str());
      ++iter;
    }

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
  }

  if (instance->request_body.length()) {
    if (instance->method == "POST") {
      curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
      curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS,
                       instance->request_body.c_str());
      curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE,
                       instance->request_body.length());
    } else {
      curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1);
      curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, &ReadFunction);
      curl_easy_setopt(curl_handle, CURLOPT_READDATA, instance);
      curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE, instance->request_body.length());
    }
  }

  if (instance->method == "DELETE") {
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
  }

  curl_easy_setopt(curl_handle, CURLOPT_URL, instance->url.c_str());
  curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, &HeaderFunction);
  curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, instance);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &WriteFunction);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, instance);

  if (!instance->verify_peer)
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);

  if (instance->username.length() && instance->password.length()) {
    std::string userpass = instance->username + ":" + instance->password;

    curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, instance->auth_method);
    curl_easy_setopt(curl_handle, CURLOPT_USERPWD, userpass.c_str());
  }

  CURLcode error = curl_easy_perform(curl_handle);

  curl_slist_free_all(headers);

  if (error != 0)
    throw URLError("curl_easy_perform() failed", error);

  instance->performed = true;
}

std::string Request::get_statusLine(Instance* instance) {
  if (!instance->performed)
    throw URLError("request not performed yet");

  return instance->status_line;
}

int Request::get_statusCode(Instance* instance) {
  if (!instance->performed)
    throw URLError("request not performed yet");

  return instance->status_code;
}

base::Object Request::get_responseHeaders(Instance* instance) {
  if (!instance->performed)
    throw URLError("request not performed yet");

  base::Object result;

  if (!instance->GetObject().HasHidden("responseHeaders")) {
    Header* header = Header::FromContext();
    std::vector<Header::Instance*> headers;

    decltype(instance->response_headers)::const_iterator
        iter(instance->response_headers.begin());

    while (iter != instance->response_headers.end()) {
      headers.push_back(header->New(iter->first, iter->second));
      ++iter;
    }

    result = base::Array::FromVector(headers);
    iter = instance->response_headers.begin();

    while (iter != instance->response_headers.end()) {
      result.Put(iter->first, iter->second);
      ++iter;
    }

    result.Freeze();

    instance->GetObject().PutHidden("responseHeaders", result);
  }
  else
    result = instance->GetObject().GetHidden("responseHeaders").AsObject();

  return result;
}

builtin::Bytes::Value Request::get_responseBody(Instance* instance) {
  if (!instance->performed)
    throw URLError("request not performed yet");

  return builtin::Bytes::FromContext()->New(instance->response_body);
}

void Request::HandleOptions(Instance* instance,
                            const utilities::Options& options) {
  if (options.Has("username") && options.Has("password")) {
    setCredentials(instance, options.GetString("username"),
                   options.GetString("password"),
                   options.GetString("authentication_method", "any"));
  }

  if (options.Has("headers")) {
    utilities::Options headers(options.GetObject("headers"));

    for (auto iter(headers.begin()); iter != headers.end(); ++iter)
      setRequestHeader(instance, iter->first, headers.GetString(iter->first));
  }

  if (options.Has("verify_peer"))
    setVerifyPeer(instance, options.GetBoolean("verify_peer"));
}

}
}


namespace conversions {

using namespace modules::url;

template <>
Request::Instance* as_value(const base::Variant& value, Request::Instance**) {
  Request* request = Request::FromContext();
  if (!value.IsObject() || !request->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected Request object");
  return Request::Instance::FromObject(request, value.AsObject());
}

template <>
base::Variant as_result(Request::Instance* result) {
  if (!result)
    return base::Variant::Null();
  return result->GetObject().handle();
}

}
