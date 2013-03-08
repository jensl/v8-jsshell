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

#ifndef MODULES_IO_SOCKETADDRESS_H
#define MODULES_IO_SOCKETADDRESS_H

#include "api/Class.h"

struct sockaddr;
struct addrinfo;

namespace modules {
namespace io {

class Module;

class SocketAddress : public api::Class {
 public:
  class Instance;

  SocketAddress();

  static bool is_valid(SocketAddress::Instance* instance);
  static int family(SocketAddress::Instance* instance);
  static struct sockaddr* addr(SocketAddress::Instance* instance);
  static size_t addrlen(SocketAddress::Instance* instance);

  Instance* New(struct sockaddr* addr, size_t addrlen);

  static SocketAddress* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(SocketAddress*);

  static Instance* unix(SocketAddress*, std::string path);
  static Instance* internet(SocketAddress*, Optional<std::string> host,
                            Optional<base::Variant> service);
  static Instance* internet6(SocketAddress*, Optional<std::string> host,
                             Optional<base::Variant> service);

  static std::string get_family(Instance* instance);
  static std::string get_unix(Instance* instance);
  static base::Object get_internet(Instance* instance);
  static base::Object get_internet6(Instance* instance);
};

}
}

namespace conversions {

using namespace modules::io;

template <>
SocketAddress::Instance* as_value(
    const base::Variant& value, SocketAddress::Instance**);

template <>
base::Variant as_result(SocketAddress::Instance* result);

}

#endif // MODULES_IO_SOCKET_H
