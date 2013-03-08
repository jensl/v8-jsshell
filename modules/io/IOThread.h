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

#ifndef MODULES_IO_IOTHREAD_H
#define MODULES_IO_IOTHREAD_H

#include <map>
#include <string>

#include "utilities/Thread.h"
#include "utilities/Mutex.h"
#include "utilities/Condition.h"

namespace modules {
namespace io {

class IOThread : public utilities::Thread {
 public:
  class FileSource;
  class SocketSource;

  class Source {
   public:
    bool IsReady();
    bool IsFailed();

   private:
    friend class IOThread;
    friend class FileSource;
    friend class SocketSource;

    Source(int fd, std::string data);
    virtual void Stop() = 0;

    utilities::Mutex mutex_;
    int fd_;
    std::string data_;
    size_t written_;
    bool stopped_;
  };

  class Sink {
   public:
    void Wait();
    std::string GetData();

   private:
    friend class IOThread;

    Sink(int fd);
    void Stop();

    utilities::Mutex mutex_;
    utilities::Condition condition_;
    int fd_;
    std::string data_;
    bool stopped_;
  };

  static Source* AddSource(int fd, std::string data);
  static Sink* AddSink(int fd);

 private:
  IOThread();

  virtual void Run() override;

  utilities::Mutex mutex_;
  bool stopped_;
  int pipe_read_;
  int pipe_write_;
  std::map<int, Source*> sources_;
  std::map<int, Sink*> sinks_;

  static IOThread* instance;
};

}
}

#endif // MODULES_IO_IOTHREAD_H
