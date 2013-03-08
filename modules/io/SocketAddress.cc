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
#include "modules/io/SocketAddress.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#include <memory>

#include "modules/IO.h"
#include "modules/io/IOError.h"

namespace modules {
namespace io {

class SocketAddress::Instance : public Class::Instance<SocketAddress> {
 public:
  Instance(struct sockaddr* addr = 0, size_t addrlen = 0);
  ~Instance();

  enum class Type {
    kInvalid = -1,
    kUnix,
    kInternet,
    kInternet6
  };

  Type type;
  struct sockaddr* sockaddr;
  struct addrinfo* addrinfo;
  size_t sockaddr_length;
};

SocketAddress::Instance::Instance(struct sockaddr* addr, size_t addrlen)
    : type(Type::kInvalid)
    , sockaddr(addr)
    , addrinfo(NULL)
    , sockaddr_length(addrlen) {
  if (addr) {
    switch (addr->sa_family) {
      case AF_UNIX:
        type = Type::kUnix;
        break;
      case AF_INET:
        type = Type::kInternet;
        break;
      case AF_INET6:
        type = Type::kInternet6;
        break;
      default:
        throw IOError("Invalid socket address family");
    }
  }
}

SocketAddress::Instance::~Instance() {
  if (type == Type::kUnix)
    delete reinterpret_cast<struct sockaddr_un*>(sockaddr);
  else if (type == Type::kInternet || type == Type::kInternet6) {
    if (addrinfo)
      freeaddrinfo(addrinfo);
    else if (sockaddr)
      delete[] reinterpret_cast<char*>(sockaddr);
  }
}

SocketAddress::SocketAddress()
    : api::Class("SocketAddress", &constructor) {
  AddProperty<SocketAddress>("family", &get_family);
  AddProperty<SocketAddress>("unix", &get_unix);
  AddProperty<SocketAddress>("internet", &get_internet);
  AddProperty<SocketAddress>("internet6", &get_internet6);

  AddClassFunction<SocketAddress>("unix", &unix);
  AddClassFunction<SocketAddress>("internet", &internet);
  AddClassFunction<SocketAddress>("internet6", &internet6);
}

bool SocketAddress::is_valid(SocketAddress::Instance* instance) {
  return instance->type != Instance::Type::kInvalid;
}

int SocketAddress::family(SocketAddress::Instance* instance) {
  switch (instance->type) {
    case Instance::Type::kUnix:
      return AF_UNIX;
    case Instance::Type::kInternet:
      return AF_INET;
    case Instance::Type::kInternet6:
      return AF_INET6;
    default:
      return -1;
  }
}

struct sockaddr* SocketAddress::addr(SocketAddress::Instance* instance) {
  return instance->sockaddr;
}

size_t SocketAddress::addrlen(SocketAddress::Instance* instance) {
  return instance->sockaddr_length;
}

SocketAddress::Instance* SocketAddress::New(struct sockaddr* addr,
                                            size_t addrlen) {
  Instance* instance = new Instance(addr, addrlen);
  instance->CreateObject(this);
  return instance;
}

SocketAddress* SocketAddress::FromContext(v8::Handle<v8::Context> context) {
  return IO::FromContext(context)->socket_address();
}

SocketAddress::Instance* SocketAddress::constructor(SocketAddress*) {
  return new Instance;
}

SocketAddress::Instance* SocketAddress::unix(
    SocketAddress* cls, std::string path) {
  std::unique_ptr<struct sockaddr_un> address(new struct sockaddr_un);

  if (path.length() + 1 > sizeof address->sun_path)
    throw IOError("path too long");

  address->sun_family = AF_UNIX;
  strcpy(address->sun_path, path.c_str());

  Instance* instance = new Instance;

  instance->type = Instance::Type::kUnix;
  instance->sockaddr = reinterpret_cast<struct sockaddr*>(address.release());
  instance->sockaddr_length = sizeof(struct sockaddr_un);
  instance->CreateObject(cls);

  return instance;
}

namespace {

SocketAddress::Instance* internet_common(SocketAddress* cls, int family,
                                         Optional<std::string> host,
                                         Optional<base::Variant> service_arg) {
  struct addrinfo hints;

  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = family;
  hints.ai_socktype = 0;
  hints.ai_protocol = 0;

  base::Variant service(service_arg.value(base::Variant::UInt32(0)));

  if (service.IsNumber())
    service = service.ToUInt32();

  struct addrinfo *addrinfo = NULL;
  const char *host_string = host.specified() ? host.value().c_str() : NULL;
  const char *service_string = service.AsString().c_str();

  int errcode = getaddrinfo(host_string, service_string, &hints, &addrinfo);

  if (errcode != 0)
    throw IOError("getaddrinfo() failed: ") << gai_strerror(errcode);

  SocketAddress::Instance* instance = new SocketAddress::Instance;

  if (family == AF_INET)
    instance->type = SocketAddress::Instance::Type::kInternet;
  else
    instance->type = SocketAddress::Instance::Type::kInternet6;

  instance->sockaddr = addrinfo->ai_addr;
  instance->addrinfo = addrinfo;
  instance->sockaddr_length = addrinfo->ai_addrlen;
  instance->CreateObject(cls);

  return instance;
}

}

SocketAddress::Instance* SocketAddress::internet(
    SocketAddress* cls, Optional<std::string> host,
    Optional<base::Variant> service) {
  return internet_common(cls, AF_INET, host, service);
}

SocketAddress::Instance* SocketAddress::internet6(
    SocketAddress* cls, Optional<std::string> host,
    Optional<base::Variant> service) {
  return internet_common(cls, AF_INET6, host, service);
}

std::string SocketAddress::get_family(Instance* instance) {
  switch (instance->type) {
    case Instance::Type::kUnix:
      return "unix";
    case Instance::Type::kInternet:
      return "internet";
    case Instance::Type::kInternet6:
      return "internet6";
    default:
      return "<invalid address>";
  }
}

std::string SocketAddress::get_unix(Instance* instance) {
  if (instance->type != Instance::Type::kUnix)
    throw IOError("Not a UNIX socket address");

  struct sockaddr_un* addr(reinterpret_cast<struct sockaddr_un*>(
      instance->sockaddr));

  return addr->sun_path;
}

base::Object SocketAddress::get_internet(Instance* instance) {
  if (instance->type != Instance::Type::kInternet)
    throw IOError("Not an Internet socket address");

  struct sockaddr_in* addr(reinterpret_cast<struct sockaddr_in*>(
      instance->sockaddr));

  base::Object result(base::Object::Create());
  char buffer[INET_ADDRSTRLEN];

  result.Put("address", base::Variant::String(inet_ntop(
      AF_INET, &addr->sin_addr, buffer, sizeof buffer)));
  result.Put("port", base::Variant::UInt32(ntohs(addr->sin_port)));

  return result;
}

base::Object SocketAddress::get_internet6(Instance* instance) {
  if (instance->type != Instance::Type::kInternet6)
    throw IOError("Not an Internet6 socket address");

  struct sockaddr_in6* addr(reinterpret_cast<struct sockaddr_in6*>(
      instance->sockaddr));

  base::Object result(base::Object::Create());
  char buffer[INET_ADDRSTRLEN];

  result.Put("address", base::Variant::String(inet_ntop(
      AF_INET6, &addr->sin6_addr, buffer, sizeof buffer)));
  result.Put("port", base::Variant::UInt32(ntohs(addr->sin6_port)));

  return result;
}

}
}

namespace conversions {

using namespace modules::io;

template <>
SocketAddress::Instance* as_value(
    const base::Variant& value, SocketAddress::Instance**) {
  SocketAddress* file = SocketAddress::FromContext();
  if (!value.IsObject() || !file->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected SocketAddress object");
  return SocketAddress::Instance::FromObject(file, value.AsObject());
}

template <>
base::Variant as_result(modules::io::SocketAddress::Instance* result) {
  return result->GetObject().handle();
}

}
