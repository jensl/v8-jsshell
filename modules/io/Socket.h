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

#ifndef MODULES_IO_SOCKET_H
#define MODULES_IO_SOCKET_H

#include "api/Class.h"

#include "modules/io/SocketAddress.h"
#include "modules/builtin/Bytes.h"

namespace modules {
namespace io {

class Module;

class Socket : public api::Class {
 public:
  class Instance;

  Socket();

  static int fd(Instance* instance);

  Instance* FromObject(base::Object object);

  static Socket* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Socket*, std::string domain, std::string type);

  static int fileno(Instance* instance);
  static void setBlocking(Instance* instance, bool value);

  static void bind(Instance* instance, SocketAddress::Instance* address);
  static void listen(Instance* instance, int backlog);
  static Socket::Instance* accept(Instance* instance);
  static void connect(Instance* instance, SocketAddress::Instance* address);
  static builtin::Bytes::Instance* recv(Instance* instance, unsigned buflen);
  static int recvfd(Instance* instance);
  static std::int64_t send(Instance* instance, builtin::Bytes::Instance* bytes);
  static void sendfd(Instance* instance, base::Variant fd);
  static void shutdown(Instance* instance, std::string how);
  static void close(Instance* instance);

  static SocketAddress::Instance* get_localAddress(Instance* instance);
  static SocketAddress::Instance* get_peerAddress(Instance* instance);
};

}
}

namespace conversions {

using namespace modules::io;

template <>
Socket::Instance* as_value(
    const base::Variant& value, Socket::Instance**);

template <>
base::Variant as_result(Socket::Instance* result);

}

#endif // MODULES_IO_SOCKET_H
