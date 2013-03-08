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
#include "modules/IO.h"

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/io/File.h"
#include "modules/io/MemoryFile.h"
#include "modules/io/Socket.h"
#include "modules/io/Poll.h"
#include "modules/io/Stat.h"
#include "modules/io/Pipe.h"
#include "modules/io/Buffered.h"
#include "modules/io/Path.h"

namespace modules {

IO::IO(const Features& features)
    : api::Module(kIO, "IO")
    , file_(new io::File)
    , memory_file_(new io::MemoryFile(file_))
    , socket_(new io::Socket)
    , socket_address_(new io::SocketAddress)
    , poll_(new io::Poll)
    , stat_(new io::Stat)
    , pipe_(new io::Pipe)
    , buffered_(new io::Buffered)
    , path_(new io::Path) {
}

IO::~IO() {
  delete file_;
  delete memory_file_;
  delete socket_;
  delete socket_address_;
  delete poll_;
  delete stat_;
  delete pipe_;
  delete buffered_;
  delete path_;
}

void IO::ExtendObject(base::Object target) {
  file_->AddTo(target);
  memory_file_->AddTo(target);
  socket_->AddTo(target);
  socket_address_->AddTo(target);
  poll_->AddTo(target);
  stat_->AddTo(target);
  pipe_->AddTo(target);
  buffered_->AddTo(target);
  path_->AddTo(target);
}

void IO::ExtendRuntime(api::Runtime& runtime) {
  AddTo(runtime.GetGlobalObject());
  ExtendObject(GetObject());
}

IO* IO::FromContext(v8::Handle<v8::Context> context) {
  return api::Module::FromContext<IO>(kIO, context);
}

}
