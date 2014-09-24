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

#ifndef MODULES_IO_MEMORYFILE_H
#define MODULES_IO_MEMORYFILE_H

#include "api/Class.h"
#include "modules/io/File.h"
#include "modules/builtin/Bytes.h"

namespace modules {
namespace io {

class Module;

class MemoryFile : public api::Class {
 public:
  class Instance;

  MemoryFile(File* file);

  static std::string value(Instance* instance);
  static void set_value(Instance* instance, std::string value);
  static std::string mode(Instance* instance);

  base::Object Create(std::string value, std::string mode);

  Instance* FromObject(base::Object object);

  static MemoryFile* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(MemoryFile*,
                               Optional<builtin::Bytes::Value> value);

  static builtin::Bytes::Value read(Instance* instance,
                                    Optional<unsigned> buflen);
  static void write(Instance* file, builtin::Bytes::Value bytes);
  static void close(Instance* file);

  static builtin::Bytes::Value get_value(Instance*);
};

}
}

#endif // MODULES_IO_MEMORYFILE_H
