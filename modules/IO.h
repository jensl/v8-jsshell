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

#ifndef MODULES_IO_H
#define MODULES_IO_H

#include "api/Module.h"

namespace modules {

class Features;

namespace io {
class File;
class MemoryFile;
class Socket;
class SocketAddress;
class Poll;
class Stat;
class Pipe;
class Buffered;
class Path;
}

class IO : public api::Module {
 public:
  IO(const Features& features);
  ~IO();

  virtual void ExtendObject(base::Object target) override;
  virtual void ExtendRuntime(api::Runtime& runtime) override;

  io::File* file() { return file_; }
  io::MemoryFile* memory_file() { return memory_file_; }
  io::Socket* socket() { return socket_; }
  io::SocketAddress* socket_address() { return socket_address_; }
  io::Poll* poll() { return poll_; }
  io::Stat* stat() { return stat_; }
  io::Pipe* pipe() { return pipe_; }
  io::Buffered* buffered() { return buffered_; }
  io::Path* path() { return path_; }

  static IO* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  io::File* file_;
  io::MemoryFile* memory_file_;
  io::Socket* socket_;
  io::SocketAddress* socket_address_;
  io::Poll* poll_;
  io::Stat* stat_;
  io::Pipe* pipe_;
  io::Buffered* buffered_;
  io::Path* path_;
};

}

#endif // MODULES_IO_H
