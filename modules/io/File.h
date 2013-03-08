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

#ifndef MODULES_IO_FILE_H
#define MODULES_IO_FILE_H

#include "api/Class.h"
#include "modules/io/Stat.h"
#include "modules/builtin/Bytes.h"
#include "utilities/FileDescriptor.h"

namespace modules {
namespace io {

class File : public api::Class {
 public:
  class Instance;

  File();

  static int Steal(Instance* instance);

  Instance* New(int fd, std::string mode);
  Instance* FromObject(base::Object object);

  static Instance* FromObject(File* module, base::Object object);

  static File* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* NewInstance(std::string path, std::string mode);

  static Instance* constructor(File*, std::string path,
                               Optional<std::string> mode);

  static int fileno(Instance* instance);
  static void setBlocking(Instance* instance, bool value);

  static builtin::Bytes::Instance* read(Instance* instance,
                                        Optional<unsigned> buflen);
  static void write(Instance* instance, builtin::Bytes::Instance* bytes);
  static Stat::Instance* stat(Instance* instance);
  static void close(Instance* instance);

  static bool get_closed(Instance* instance);

  static builtin::Bytes::Instance* read(File*, std::string path);
  static void write(File*, std::string path, builtin::Bytes::Instance* bytes);

  static File::Instance* fdopen(File* module, utilities::FileDescriptor fd,
                                Optional<std::string> mode);
  static Stat::Instance* stat(File* module, std::string path);
  static int dup(File*, utilities::FileDescriptor fd);
  static void dup2(File*, utilities::FileDescriptor oldfd, int newfd);
  static void link(File*, std::string old_path, std::string new_path);
  static void symlink(File*, std::string old_path, std::string new_path);
  static void unlink(File*, std::string path);
  static void chmod(File*, std::string path, int mode);
  static void rename(File*, std::string old_path, std::string new_path);
  static void chdir(File*, std::string path);
  static void mkdir(File*, std::string path, Optional<int> mode_arg);
  static void utimes(File*, std::string path, Optional<double> atime,
                     Optional<double> mtime);

  static base::Object listDirectory(File* module, std::string path);

  template <unsigned Flag>
  static bool is(File* module, std::string path);

  static Instance* get_stdin(File* module);
  static Instance* get_stdout(File* module);
  static Instance* get_stderr(File* module);

  base::Object::Persistent stdin;
  base::Object::Persistent stdout;
  base::Object::Persistent stderr;
};

}
}

namespace conversions {

using namespace modules::io;

template <>
File::Instance* as_value(const base::Variant& value, File::Instance**);

template <>
base::Variant as_result(File::Instance* result);

}

#endif // MODULES_IO_FILE_H
