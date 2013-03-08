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

#ifndef MODULES_ZLIB_H
#define MODULES_ZLIB_H

#if ZLIB_SUPPORT

#include "api/Module.h"
#include "modules/builtin/Bytes.h"

namespace api {
class Runtime;
}

namespace modules {

class Features;

namespace zlib {
class Stream;
}

class ZLib : public api::Module {
 public:
  ZLib(const Features& features);
  ~ZLib();

  virtual void ExtendObject(base::Object target) override;
  virtual void ExtendRuntime(api::Runtime& runtime) override;

  zlib::Stream* stream() { return stream_; }

  static ZLib* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  zlib::Stream* stream_;

  static builtin::Bytes::Instance* deflate(
      ZLib*, builtin::Bytes::Instance* data, Optional<int> level);
  static builtin::Bytes::Instance* inflate(
      ZLib*, builtin::Bytes::Instance* data);
};

}

#endif // ZLIB_SUPPORT
#endif // MODULES_ZLIB_H
