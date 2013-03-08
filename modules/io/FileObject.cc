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
#include "modules/io/FileObject.h"

namespace modules {
namespace io {

FileObject::FileObject() {
}

FileObject::FileObject(const FileObject& other)
    : object_(other.object_) {
}

FileObject::FileObject(base::Object object)
    : object_(object) {
}

bool FileObject::IsFile() {
  return !object_.IsEmpty() && File::FromContext()->HasInstance(object_);
}

File::Instance* FileObject::GetFile() {
  if (!IsFile())
    return NULL;
  return File::FromContext()->FromObject(object_);
}

bool FileObject::IsMemoryFile() {
  return !object_.IsEmpty() && MemoryFile::FromContext()->HasInstance(object_);
}

MemoryFile::Instance* FileObject::GetMemoryFile() {
  if (!IsMemoryFile())
    return NULL;
  return MemoryFile::FromContext()->FromObject(object_);
}

bool FileObject::IsSocket() {
  return !object_.IsEmpty() && Socket::FromContext()->HasInstance(object_);
}

Socket::Instance* FileObject::GetSocket() {
  if (!IsSocket())
    return NULL;
  return Socket::FromContext()->FromObject(object_);
}

}
}

namespace conversions {

using namespace modules::io;

template <>
FileObject as_value(const base::Variant& value, FileObject*) {
  if (!value.IsObject())
    throw base::TypeError("invalid argument, expected object");
  File* file = File::FromContext();
  MemoryFile* memory_file = MemoryFile::FromContext();
  if (!file->HasInstance(value.AsObject()) &&
      !memory_file->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected file object");
  return FileObject(value.AsObject());
}

template <>
base::Variant as_result(FileObject result) {
  return base::Variant::Object(result.object());
}

}
