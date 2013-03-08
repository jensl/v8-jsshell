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

#ifndef MODULES_IO_FILEOBJECT_H
#define MODULES_IO_FILEOBJECT_H

#include "modules/io/File.h"
#include "modules/io/MemoryFile.h"
#include "modules/io/Socket.h"

namespace modules {
namespace io {

class FileObject {
 public:
  FileObject();
  FileObject(const FileObject& other);
  FileObject(base::Object object);

  bool IsFile();
  File::Instance* GetFile();

  bool IsMemoryFile();
  MemoryFile::Instance* GetMemoryFile();

  bool IsSocket();
  Socket::Instance* GetSocket();

  base::Object object() { return object_; }

 private:
  base::Object object_;
};

}
}

namespace conversions {

using namespace modules::io;

template <>
FileObject as_value(const base::Variant& value, FileObject*);

template <>
base::Variant as_result(FileObject result);

}

#endif // MODULES_IO_FILEOBJECT_H
