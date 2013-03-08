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

#ifndef MODULES_IO_STAT_H
#define MODULES_IO_STAT_H

#include <sys/types.h>
#include <sys/stat.h>

#include "api/Class.h"

namespace modules {
namespace io {

class Stat : public api::Class {
 public:
  class Instance;

  Stat();

  Instance* New(const struct stat& data);

  static Stat* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  template <unsigned Flag>
  static bool is(Instance* instance);

  static std::int64_t get_dev(Instance* instance);
  static std::int64_t get_ino(Instance* instance);
  static std::int64_t get_mode(Instance* instance);
  static std::int64_t get_nlink(Instance* instance);
  static std::int64_t get_uid(Instance* instance);
  static std::int64_t get_gid(Instance* instance);
  static std::int64_t get_rdev(Instance* instance);
  static std::int64_t get_size(Instance* instance);
  static std::int64_t get_blksize(Instance* instance);
  static std::int64_t get_blocks(Instance* instance);
  static base::Object get_atime(Instance* instance);
  static base::Object get_mtime(Instance* instance);
  static base::Object get_ctime(Instance* instance);
};

}
}

namespace conversions {

using namespace modules::io;

template <>
Stat::Instance* as_value(const base::Variant& value, Stat::Instance**);

template <>
base::Variant as_result(Stat::Instance* result);

}

#endif // MODULES_IO_STAT_H
