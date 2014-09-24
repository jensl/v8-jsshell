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
#include "modules/io/Buffered.h"

#include <limits>

#include "modules/io/IOError.h"
#include "utilities/FileDescriptor.h"

namespace modules {
namespace io {

class Buffered::Instance : public api::Class::Instance<Buffered> {
 public:
  Instance(int fd);

  std::string buffer;
  int fd;
  bool eof;
};

Buffered::Instance::Instance(int fd)
    : fd(fd)
    , eof(false) {
}

Buffered::Buffered()
    : api::Class("Buffered", constructor) {
  AddMethod<Buffered>("read", read);
  AddMethod<Buffered>("readln", readln);
}

Buffered::Instance* Buffered::constructor(Buffered*, base::Variant source) {
  if (source.IsObject())
    source = source.AsObject().Call("fileno");

  return new Instance(source.AsInt32());
}

namespace {

void Fill(Buffered::Instance* instance) {
  try {
    instance->buffer += utilities::FileDescriptor::ReadSome(instance->fd, 8172,
							    true);
  } catch (utilities::FileDescriptor::EndOfFile&) {
    instance->eof = true;
  } catch (utilities::FileDescriptor::Error& error) {
    throw IOError("read() failed", error.errnum);
  }
}

}

builtin::Bytes::Value Buffered::read(Instance* instance,
                                     Optional<unsigned> nbytes_opt) {
  size_t nbytes;
  if (nbytes_opt.specified())
    nbytes = nbytes_opt.value();
  else
    nbytes = std::numeric_limits<size_t>::max();
  while (instance->buffer.length() < nbytes && !instance->eof)
    Fill(instance);
  std::string result;
  if (nbytes_opt.specified()) {
    result = instance->buffer.substr(0, nbytes);
    instance->buffer = instance->buffer.substr(nbytes);
  } else {
    result = instance->buffer;
    instance->buffer = "";
  }
  return builtin::Bytes::FromContext()->New(result);
}

base::Variant Buffered::readln(Instance* instance) {
  std::string::size_type pos = 0;

  if (instance->buffer.length() == 0)
    Fill(instance);

  while (pos < instance->buffer.length()) {
    pos = instance->buffer.find("\n", pos);

    if (pos != std::string::npos) {
      std::string result(instance->buffer.substr(0, pos));
      instance->buffer = instance->buffer.substr(pos + 1);
      return base::Variant::String(result);
    }

    pos = instance->buffer.length();

    Fill(instance);
  }

  return base::Variant::Null();
}

}
}
