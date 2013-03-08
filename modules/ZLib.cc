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

#if ZLIB_SUPPORT

#include "Base.h"
#include "modules/ZLib.h"

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/zlib/Stream.h"

namespace modules {

ZLib::ZLib(const Features& features)
    : api::Module(kZLib, "ZLib")
    , stream_(new zlib::Stream) {
}

ZLib::~ZLib() {
  delete stream_;
}

void ZLib::ExtendObject(base::Object target) {
  stream_->AddTo(target);

  AddFunction(target, "deflate", deflate);
  AddFunction(target, "inflate", inflate);
}

void ZLib::ExtendRuntime(api::Runtime& runtime) {
  AddTo(runtime.GetGlobalObject());
  ExtendObject(GetObject());
}

ZLib* ZLib::FromContext(v8::Handle<v8::Context> context) {
  return api::Module::FromContext<ZLib>(kZLib, context);
}

builtin::Bytes::Instance* ZLib::deflate(ZLib*, builtin::Bytes::Instance* data,
                                        Optional<int> level_opt) {
  int level;

  if (level_opt.specified())
    level = level_opt.value();
  else
    level = -1;

  return zlib::Stream::Deflate(data, level);
}

builtin::Bytes::Instance* ZLib::inflate(ZLib*, builtin::Bytes::Instance* data) {
  return zlib::Stream::Inflate(data);
}

}

#endif // ZLIB_SUPPORT
