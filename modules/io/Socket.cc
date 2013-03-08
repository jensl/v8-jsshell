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
#include "modules/io/Socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#include <memory>
#include <cstring>

#include "modules/IO.h"
#include "modules/io/IOError.h"

namespace modules {
namespace io {

class Socket::Instance : public Class::Instance<Socket> {
 public:
  Instance();
  Instance(int domain, int type, int fd);
  ~Instance();

  int domain;
  int type;
  int fd;
  bool listening;
  bool connected;
};

Socket::Instance::Instance()
    : fd(-1)
    , listening(false)
    , connected(false) {
}

Socket::Instance::Instance(int domain, int type, int fd)
    : domain(domain)
    , type(type)
    , fd(fd)
    , listening(false)
    , connected(false) {
}

Socket::Instance::~Instance() {
  if (fd != -1) {
    fprintf(stderr, "WARNING: open socket garbage collected\n");
    /* Note: ignoring failure to close here; no practical way to
       signal it anyway. */
    ::close(fd);
  }
}

Socket::Socket()
    : api::Class("Socket", constructor) {
  AddMethod<Socket>("fileno", fileno);
  AddMethod<Socket>("setBlocking", setBlocking);
  AddMethod<Socket>("bind", bind);
  AddMethod<Socket>("listen", listen);
  AddMethod<Socket>("accept", accept);
  AddMethod<Socket>("connect", connect);
  AddMethod<Socket>("recv", recv);
  AddMethod<Socket>("recvfd", recvfd);
  AddMethod<Socket>("send", send);
  AddMethod<Socket>("sendfd", sendfd);
  AddMethod<Socket>("shutdown", shutdown);
  AddMethod<Socket>("close", close);

  AddProperty<Socket>("localAddress", get_localAddress);
  AddProperty<Socket>("peerAddress", get_peerAddress);
}

int Socket::fd(Instance* instance) {
  return instance->fd;
}

Socket::Instance* Socket::FromObject(base::Object object) {
  return Instance::FromObject(this, object);
}

Socket* Socket::FromContext(v8::Handle<v8::Context> context) {
  return IO::FromContext(context)->socket();
}

Socket::Instance* Socket::constructor(Socket*, std::string domain_in,
                                      std::string type_in) {
  int domain;

  if (domain_in == "unix" || domain_in == "AF_UNIX")
    domain = AF_UNIX;
  else if (domain_in == "internet" || domain_in == "AF_INET")
    domain = AF_INET;
  else if (domain_in == "internet6" || domain_in == "AF_INET6")
    domain = AF_INET6;
  else
    throw IOError("unsupported domain argument: ") << domain_in;

  int type;

  if (type_in == "stream" || type_in == "SOCK_STREAM")
    type = SOCK_STREAM;
  else if (type_in == "datagram" || type_in == "SOCK_DGRAM")
    type = SOCK_DGRAM;
  else
    throw IOError("unsupported type argument: ") << type_in;

  int fd = socket(domain, type, 0);

  if (fd == -1)
    throw IOError("socket() failed", errno);

  return new Instance(domain, type, fd);
}

int Socket::fileno(Instance* instance) {
  return instance->fd;
}

void Socket::setBlocking(Instance* instance, bool value) {
  int flags = ::fcntl(instance->fd, F_GETFL);

  if (flags == -1)
    throw IOError("fnctl() failed", errno);

  int new_flags;

  if (!value)
    new_flags = flags | O_NONBLOCK;
  else
    new_flags = flags & ~O_NONBLOCK;

  if (new_flags != flags) {
    int retval = ::fcntl(instance->fd, F_SETFL, new_flags);

    if (retval == -1)
      throw IOError("fnctl() failed", errno);
  }
}

void Socket::bind(Instance* instance, SocketAddress::Instance* address) {
  if (instance->connected)
    throw IOError("socket already connected");
  else if (instance->listening)
    throw IOError("socket already listening");
  else if (!SocketAddress::is_valid(address))
    throw IOError("invalid address");
  else if (SocketAddress::family(address) != instance->domain)
    throw IOError("wrong address type");

  int retval = ::bind(instance->fd, SocketAddress::addr(address),
                      SocketAddress::addrlen(address));

  if (retval == -1)
    throw IOError("bind() failed", errno);
}

void Socket::listen(Instance* instance, int backlog) {
  if (instance->connected)
    throw IOError("socket already connected");
  else if (instance->listening)
    throw IOError("socket already listening");

  int retval = ::listen(instance->fd, backlog);

  if (retval == -1)
    throw IOError("listen() failed", errno);

  instance->listening = true;
}

Socket::Instance* Socket::accept(Instance* instance) {
  if (instance->connected)
    throw IOError("socket already connected");
  else if (!instance->listening)
    throw IOError("socket not listening");

  int fd = ::accept(instance->fd, NULL, NULL);

  if (fd == -1)
    throw IOError("listen() failed", errno);

  Instance* new_instance = new Instance(instance->domain, instance->type, fd);
  new_instance->CreateObject(instance->GetClass());
  new_instance->connected = true;
  return new_instance;
}

void Socket::connect(Socket::Instance* instance,
                     SocketAddress::Instance* address) {
  if (instance->connected)
    throw IOError("socket already connected");
  else if (instance->listening)
    throw IOError("socket already listening");
  else if (!SocketAddress::is_valid(address))
    throw IOError("invalid address");
  else if (SocketAddress::family(address) != instance->domain)
    throw IOError("wrong address type");

  int retval = ::connect(instance->fd, SocketAddress::addr(address),
                         SocketAddress::addrlen(address));

  if (retval == -1)
    throw IOError("connect() failed", errno);

  instance->connected = true;
}

builtin::Bytes::Instance* Socket::recv(Socket::Instance* instance,
                                       unsigned buflen) {
  if (!instance->connected)
    throw IOError("socket not connected");
  else if (buflen == 0)
    throw IOError("zero buffer length specified");

  std::unique_ptr<char[]> buffer(new char[buflen]);
  ssize_t retval = ::recv(instance->fd, buffer.get(), buflen, 0);

  if (retval == 0)
    return NULL;

  if (retval == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
      retval = 0;
    else
      throw IOError("recv() failed", errno);
  }

  return builtin::Bytes::FromContext()->New(std::string(buffer.get(), retval));
}

int Socket::recvfd(Instance* instance) {
  if (instance->domain != AF_UNIX)
    throw IOError("recvfd() only available on UNIX sockets");
  else if (!instance->connected)
    throw IOError("socket not connected");

  struct iovec iov[1];
  struct msghdr msgh;
  struct cmsghdr* h;

  char buf[CMSG_SPACE(sizeof(int))];
  char msgbuf[1024];

  std::memset(&iov[0], 0, sizeof(struct iovec));
  std::memset(&msgh, 0, sizeof(struct msghdr));
  std::memset(buf, 0, sizeof buf);

  msgh.msg_name = 0;
  msgh.msg_namelen = 0;
  msgh.msg_iov = iov;
  msgh.msg_iovlen = 1;
  msgh.msg_control = buf;
  msgh.msg_controllen = sizeof buf;
  msgh.msg_flags = 0;

  iov[0].iov_base = msgbuf;
  iov[0].iov_len = sizeof msgbuf;

  if (recvmsg(instance->fd, &msgh, 0) == -1)
    throw IOError("recvmsg() failed", errno);

  h = CMSG_FIRSTHDR(&msgh);

  if (!h || h->cmsg_len != CMSG_LEN(sizeof(int)) ||
      h->cmsg_level != SOL_SOCKET ||
      h->cmsg_type != SCM_RIGHTS)
    throw IOError("recvmsg(): malformed message received");

  return reinterpret_cast<int*>(CMSG_DATA(h))[0];
}

std::int64_t Socket::send(Socket::Instance* instance, builtin::Bytes::Instance* bytes) {
  if (!instance->connected)
    throw IOError("socket not connected");

  const char* buffer = builtin::Bytes::data(bytes).c_str();
  size_t length = builtin::Bytes::data(bytes).length();
  size_t sent = 0;

  while (sent < length) {
    ssize_t retval =
        ::send(instance->fd, buffer + sent, length - sent, MSG_NOSIGNAL);

    if (retval == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      else
        throw IOError("send() failed", errno);
    }

    sent += retval;
  }

  return sent;
}

void Socket::sendfd(Instance* instance, base::Variant fd_arg) {
  if (instance->domain != AF_UNIX)
    throw IOError("sendfd() only available on UNIX sockets");
  else if (!instance->connected)
    throw IOError("socket not connected");

  if (fd_arg.IsObject())
    fd_arg = fd_arg.AsObject().Call("fileno");

  int fd = fd_arg.AsInt32();

  struct iovec iov[1];
  struct msghdr msgh;
  struct cmsghdr* h;

  char buf[CMSG_SPACE(sizeof(int))];

  std::memset(&iov[0], 0, sizeof(struct iovec));
  std::memset(&msgh, 0, sizeof(struct msghdr));
  std::memset(buf, 0, sizeof buf);

  msgh.msg_name = 0;
  msgh.msg_namelen = 0;
  msgh.msg_iov = iov;
  msgh.msg_iovlen = 1;
  msgh.msg_control = buf;
  msgh.msg_controllen = sizeof buf;
  msgh.msg_flags = 0;

  iov[0].iov_base = const_cast<char*>("sendfd");
  iov[0].iov_len = 6;

  h = CMSG_FIRSTHDR(&msgh);
  h->cmsg_len = CMSG_LEN(sizeof(int));
  h->cmsg_level = SOL_SOCKET;
  h->cmsg_type = SCM_RIGHTS;

  reinterpret_cast<int*>(CMSG_DATA(h))[0] = fd;

  if (sendmsg(instance->fd, &msgh, 0) == -1)
    throw IOError("sendmsg() failed", errno);
}

void Socket::shutdown(Socket::Instance* instance, std::string how_in) {
  if (!instance->connected)
    throw IOError("socket not connected");

  int how;

  if (how_in == "read" || how_in == "SHUT_RD")
    how = SHUT_RD;
  else if (how_in == "write" || how_in == "SHUT_WR")
    how = SHUT_WR;
  else if (how_in == "both" || how_in == "SHUT_RDWR")
    how = SHUT_RDWR;
  else
    throw IOError("unsupported how argument: ") << how_in;

  if (::shutdown(instance->fd, how) == -1)
    throw IOError("shutdown() failed", errno);
}

void Socket::close(Socket::Instance* instance) {
  if (instance->fd == -1)
    throw IOError("socket already closed");

  while (true) {
    if (::close(instance->fd) == -1) {
      if (errno == EINTR)
        continue;
      else
        throw IOError("close() failed", errno);
    } else {
      break;
    }
  }

  instance->fd = -1;
}

SocketAddress::Instance* Socket::get_localAddress(Instance* instance) {
  socklen_t addrlen = 0;

  if (getsockname(instance->fd, NULL, &addrlen) == -1)
    throw IOError("getsockname() failed (1)", errno);

  struct sockaddr* addr(reinterpret_cast<struct sockaddr*>(new char[addrlen]));

  if (getsockname(instance->fd, addr, &addrlen) == -1)
    throw IOError("getsockname() failed (2)", errno);

  return SocketAddress::FromContext()->New(addr, addrlen);
}

SocketAddress::Instance* Socket::get_peerAddress(Instance* instance) {
  socklen_t addrlen = 0;

  if (getpeername(instance->fd, NULL, &addrlen) == -1)
    throw IOError("getpeername() failed", errno);

  struct sockaddr* addr(reinterpret_cast<struct sockaddr*>(new char[addrlen]));

  if (getpeername(instance->fd, addr, &addrlen) == -1)
    throw IOError("getpeername() failed", errno);

  return SocketAddress::FromContext()->New(addr, addrlen);
}

}
}

namespace conversions {

using namespace modules::io;

template <>
Socket::Instance* as_value(
    const base::Variant& value, Socket::Instance**) {
  Socket* socket = Socket::FromContext();
  if (!value.IsObject() || !socket->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected Socket object");
  return Socket::Instance::FromObject(socket, value.AsObject());
}

template <>
base::Variant as_result(Socket::Instance* result) {
  return result->GetObject().handle();
}

}
