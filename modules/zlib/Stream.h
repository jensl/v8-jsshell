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

#ifndef MODULES_ZLIB_STREAM_H
#define MODULES_ZLIB_STREAM_H

#if ZLIB_SUPPORT

#include "api/Class.h"
#include "modules/builtin/Bytes.h"
#include "utilities/Options.h"

namespace modules {

class ZLib;

namespace zlib {

class Stream : public api::Class {
 public:
  class Instance;

  Stream();

  static builtin::Bytes::Instance* Deflate(builtin::Bytes::Instance* data,
                                           int level);
  static builtin::Bytes::Instance* Inflate(builtin::Bytes::Instance* data);

 private:
  static Instance* constructor(Stream*, std::string type,
                               utilities::Options options);

  static void write(Instance* instance, builtin::Bytes::Instance* data);
  static void flush(Instance* instance);
  static void close(Instance* instance);

  static builtin::Bytes::Instance* read(Instance* instance);
};

}
}

#endif // ZLIB_SUPPORT
#endif // MODULES_ZLIB_STREAM_H
