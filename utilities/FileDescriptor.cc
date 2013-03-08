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
#include "utilities/FileDescriptor.h"

#include <unistd.h>
#include <fcntl.h>

namespace utilities {

std::string FileDescriptor::ReadSome(int fd, size_t nbytes, bool async) {
  std::string result;
  char buffer[8172];

  while (nbytes == 0 || result.length() < nbytes) {
    size_t nmax = sizeof buffer;

    if (nbytes != 0 && nbytes - result.length() < nmax)
      nmax = nbytes - result.length();

    ssize_t nread = ::read(fd, buffer, nmax);

    if (nread == -1) {
      if (errno == EINTR)
        continue;
      else if (errno == EAGAIN || errno == EWOULDBLOCK)
        return result;
      else
        throw Error(errno);
    }

    if (nread == 0)
      break;

    result.append(buffer, nread);

    if (async)
      break;
  }

  if (!result.length())
    throw EndOfFile();

  return result;
}

std::string FileDescriptor::ReadAll(int fd) {
  return ReadSome(fd, 0);
}

bool FileDescriptor::IsValid(int fd) {
  return ::fcntl(fd, F_GETFL) == 0 || errno != EBADF;
}

}

namespace conversions {

template <>
utilities::FileDescriptor as_value(const base::Variant& value_in,
                                   utilities::FileDescriptor*) {
  if (value_in.IsNull() || value_in.IsUndefined())
    return -1;
  base::Variant value;
  if (value_in.IsObject())
    value = value_in.AsObject().Call("fileno");
  else
    value = value_in;
  int fd = value.AsInt32();
  if (!utilities::FileDescriptor::IsValid(fd))
    throw base::TypeError("Invalid file descriptor");
  return fd;
}

}
