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
#include "modules/io/MemoryFile.h"

#include "modules/IO.h"
#include "modules/io/IOError.h"

namespace modules {
namespace io {

class MemoryFile::Instance : public api::Class::Instance<MemoryFile> {
 public:
  Instance();
  Instance(std::string value, std::string mode);

  std::string value;
  std::string mode;
  unsigned position;
};

MemoryFile::Instance::Instance()
    : position(0) {
}

MemoryFile::Instance::Instance(std::string value, std::string mode)
    : value(value)
    , mode(mode)
    , position(0) {
}

MemoryFile::MemoryFile(File* file)
    : api::Class("MemoryFile", &constructor) {
  //Inherit(file);

  AddMethod<MemoryFile>("read", &read);
  AddMethod<MemoryFile>("write", &write);
  AddProperty<MemoryFile>("value", &get_value);
}

std::string MemoryFile::value(Instance* instance) {
  return instance->value;
}

void MemoryFile::set_value(Instance* instance, std::string value) {
  instance->value = value;
}

std::string MemoryFile::mode(Instance* instance) {
  return instance->mode;
}

base::Object MemoryFile::Create(std::string value, std::string mode) {
  Instance* instance = new Instance(value, mode);
  instance->CreateObject(this);
  return instance->GetObject();
}

MemoryFile::Instance* MemoryFile::FromObject(base::Object object) {
  return Instance::FromObject(this, object);
}

MemoryFile* MemoryFile::FromContext(v8::Handle<v8::Context> context) {
  return IO::FromContext(context)->memory_file();
}

MemoryFile::Instance* MemoryFile::constructor(
    MemoryFile*, Optional<builtin::Bytes::Instance*> value_in) {
  std::string value, mode;
  if (value_in.specified()) {
    value = builtin::Bytes::data(value_in.value());
    mode = "r";
  } else {
    mode = "a";
  }
  return new Instance(value, mode);
}

builtin::Bytes::Instance* MemoryFile::read(Instance* instance,
                                           Optional<unsigned> buflen) {
  if (instance->mode != "r")
    throw IOError("file not open for reading");
  unsigned length = instance->value.length() - instance->position;
  if (buflen.specified() && buflen.value() != 0 && buflen.value() < length)
    length = buflen.value();
  if (length == 0)
    return NULL;
  std::string result = std::string(instance->value, instance->position, length);
  instance->position += length;
  return builtin::Bytes::FromContext()->New(result);
}

void MemoryFile::write(Instance* instance, builtin::Bytes::Instance* bytes) {
  if (instance->mode != "a")
    throw IOError("file not open for reading");
  instance->value += builtin::Bytes::data(bytes);
}

void MemoryFile::close(Instance* file) {
}

builtin::Bytes::Instance* MemoryFile::get_value(Instance* instance) {
  return builtin::Bytes::FromContext()->New(instance->value);
}

}
}
