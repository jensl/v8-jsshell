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
#include "modules/io/IOThread.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "modules/io/IOError.h"

namespace modules {
namespace io {

bool IOThread::Source::IsReady() {
  utilities::Mutex::Lock lock(mutex_);
  return stopped_ && written_ == data_.length();
}

bool IOThread::Source::IsFailed() {
  utilities::Mutex::Lock lock(mutex_);
  return stopped_ && written_ < data_.length();
}

IOThread::Source::Source(int fd, std::string data)
    : fd_(fd)
    , data_(data)
    , written_(0)
    , stopped_(false) {
}

class IOThread::FileSource : public IOThread::Source {
 public:
  FileSource(int fd, std::string data)
      : IOThread::Source(fd, data) {
  }

  virtual void Stop() override {
    ::close(fd_);
  }
};

class IOThread::SocketSource : public IOThread::Source {
 public:
  SocketSource(int fd, std::string data)
      : IOThread::Source(fd, data) {
  }

  virtual void Stop() override {
    ::shutdown(fd_, SHUT_WR);
  }
};

IOThread::Sink::Sink(int fd)
    : condition_(mutex_)
    , fd_(fd)
    , stopped_(false) {
}

void IOThread::Sink::Wait() {
  utilities::Condition::Lock lock(condition_);
  while (!stopped_)
    lock.Wait();
  ::close(fd_);
}

std::string IOThread::Sink::GetData() {
  utilities::Mutex::Lock lock(mutex_);
  return data_;
}

IOThread::Source* IOThread::AddSource(int fd, std::string data) {
  struct stat buf;

  if (fstat(fd, &buf) == -1)
    throw IOError("fstat() failed", errno);

  if (!instance)
    instance = new IOThread;

  Source* source;

  if (S_ISSOCK(buf.st_mode))
    source = new SocketSource(fd, data);
  else
    source = new FileSource(fd, data);

  utilities::Mutex::Lock lock(instance->mutex_);

  instance->sources_[fd] = source;

  ::write(instance->pipe_write_, "\0", 1);

  return source;
}

IOThread::Sink* IOThread::AddSink(int fd) {
  int flags = fcntl(fd, F_GETFL);

  if (flags == -1)
    throw IOError("fcntl() failed", errno);
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    throw IOError("fcntl() failed", errno);

  if (!instance)
    instance = new IOThread;

  Sink* sink = new Sink(fd);

  utilities::Mutex::Lock lock(instance->mutex_);

  instance->sinks_[fd] = sink;

  ::write(instance->pipe_write_, "\0", 1);

  return sink;
}

IOThread::IOThread()
    : stopped_(false) {
  int fds[2];

  if (pipe(fds) == -1)
    throw IOError("pipe() failed", errno);

  pipe_read_ = fds[0];
  pipe_write_ = fds[1];

  Start();
}

namespace {

struct pollfd PollRead(int fd) {
  struct pollfd pollfd;
  pollfd.fd = fd;
  pollfd.events = POLLIN;
  pollfd.revents = 0;
  return pollfd;
}

struct pollfd PollWrite(int fd) {
  struct pollfd pollfd;
  pollfd.fd = fd;
  pollfd.events = POLLOUT;
  pollfd.revents = 0;
  return pollfd;
}

}

void IOThread::Run() {
  utilities::Mutex::Lock lock(mutex_);
  
  while (!stopped_) {
    std::vector<struct pollfd> fds;

    fds.push_back(PollRead(pipe_read_));

    for (std::map<int, Source*>::iterator source = sources_.begin();
         source != sources_.end();
         ++source)
      fds.push_back(PollWrite(source->first));

    for (std::map<int, Sink*>::iterator sink = sinks_.begin();
         sink != sinks_.end();
         ++sink)
      fds.push_back(PollRead(sink->first));

    lock.Release();

    int retval = ::poll(fds.data(), fds.size(), -1);

    if (retval == -1) {
      perror("poll() failed in IO thread");
      exit(EXIT_FAILURE);
    }

    lock.Acquire();

    std::vector<struct pollfd>::iterator pollfd(fds.begin());
    char buffer[4096];

    /* First fd is the pipe used to wake us up when new sources or sinks are
       added.  We don't care about the actual data; just read it and forget
       about it. */
    if ((pollfd->revents & POLLIN) != 0)
      ::read(pollfd->fd, buffer, sizeof buffer);
    ++pollfd;

    while (pollfd != fds.end()) {
      if ((pollfd->revents & POLLNVAL) != 0) {
        std::map<int, Source*>::iterator source(sources_.find(pollfd->fd));
        if (source != sources_.end())
          pollfd->revents = POLLOUT;

        std::map<int, Sink*>::iterator sink(sinks_.find(pollfd->fd));
        if (sink != sinks_.end())
          pollfd->revents = POLLIN;
      }

      if ((pollfd->revents & (POLLOUT | POLLERR | POLLHUP)) != 0) {
        std::map<int, Source*>::iterator source(sources_.find(pollfd->fd));

        if (source != sources_.end()) {
          ssize_t retval = ::write(
              source->second->fd_,
              source->second->data_.c_str() + source->second->written_,
              source->second->data_.length() - source->second->written_);

          utilities::Mutex::Lock lock(source->second->mutex_);

          if (retval > 0)
            source->second->written_ += retval;

          if (retval < 0)
            perror("write() failed");

          if (source->second->written_ == source->second->data_.length() ||
              (retval == -1 && errno != EINTR)) {
            source->second->Stop();
            source->second->stopped_ = true;
            sources_.erase(source);
          }
        }
      }

      if ((pollfd->revents & (POLLIN | POLLERR | POLLHUP)) != 0) {
        std::map<int, Sink*>::iterator sink(sinks_.find(pollfd->fd));

        if (sink != sinks_.end()) {
          while (true) {
            ssize_t retval = ::read(sink->second->fd_, buffer, sizeof buffer);

            utilities::Condition::Lock lock(sink->second->condition_);

            if (retval > 0)
              sink->second->data_ += std::string(buffer, retval);

            if (retval < 0) {
              if (errno == EWOULDBLOCK)
                break;
              else if (errno != EINTR)
                perror("read() failed");
            }

            if (retval == 0 || (retval == -1 && errno != EINTR)) {
              sink->second->stopped_ = true;
              lock.Signal();
              sink = sinks_.erase(sink);
              break;
            }
          }
        }
      }

      ++pollfd;
    }
  }
}

IOThread* IOThread::instance = NULL;

}
}
