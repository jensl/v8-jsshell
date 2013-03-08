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
#include "modules/io/Poll.h"

#include <poll.h>

#include <map>
#include <vector>

#include "modules/io/IOError.h"

namespace modules {
namespace io {

class Poll::Instance : public api::Class::Instance<Poll> {
 public:
  std::map<int, base::Object::Persistent> objects;
  std::vector<struct pollfd> fds;

  base::Object::Persistent read;
  base::Object::Persistent write;
};

Poll::Poll()
    : api::Class("Poll", &constructor) {
  AddMethod<Poll>("register", &register_);
  AddMethod<Poll>("unregister", &unregister);
  AddMethod<Poll>("poll", &poll);
  AddMethod<Poll>("isEmpty", &isEmpty);
  AddMethod<Poll>("clear", &clear);

  AddProperty<Poll>("read", &get_read);
  AddProperty<Poll>("write", &get_write);
}

Poll::Instance* Poll::constructor(Poll*) {
  return new Instance;
}

void Poll::register_(Instance* instance, base::Variant file,
                     Optional<std::string> mode_opt) {
  base::Object::Persistent object;

  if (file.IsObject()) {
    object = file.AsObject();
    file = file.AsObject().Call("fileno");
  }

  int fileno = file.AsInt32();

  if (fileno < 0)
    throw IOError("invalid file descriptor");

  if (instance->objects.find(fileno) != instance->objects.end())
    throw IOError("file descriptor already registered");

  short events;

  if (mode_opt.specified()) {
    if (mode_opt.value() == "read")
      events = POLLIN;
    else if (mode_opt.value() == "write")
      events = POLLOUT;
    else if (mode_opt.value() == "both")
      events = POLLIN | POLLOUT;
    else
      throw IOError("invalid mode argument: ") << mode_opt.value();
  } else {
    events = POLLIN | POLLOUT;
  }

  struct pollfd fd;

  fd.fd = fileno;
  fd.events = events;
  fd.revents = 0;

  instance->fds.push_back(fd);
  instance->objects.insert(std::make_pair(fileno, object));
}

void Poll::unregister(Instance* instance, base::Variant file) {
  base::Object::Persistent object;

  if (file.IsObject()) {
    object = file.AsObject();
    file = file.AsObject().Call("fileno");
  }

  int fileno = file.AsInt32();

  if (fileno < 0)
    throw IOError("invalid file descriptor");

  if (instance->objects.erase(fileno) == 0)
    throw IOError("file descriptor not registered");

  std::vector<struct pollfd>::iterator iter(instance->fds.begin());
  while (iter->fd != fileno)
    ++iter;
  instance->fds.erase(iter);

  instance->read.Release();
  instance->write.Release();
}

bool Poll::poll(Instance* instance, Optional<int> timeout_opt) {
  instance->read.Release();
  instance->write.Release();

  int timeout;

  if (timeout_opt.specified())
    timeout = timeout_opt.value();
  else
    timeout = -1;

  int result = ::poll(instance->fds.data(), instance->fds.size(), timeout);

  if (result == 0)
    return false;
  else if (result == -1)
    throw IOError("poll() failed", errno);

  return true;
}

bool Poll::isEmpty(Instance* instance) {
  return instance->fds.size() == 0;
}

void Poll::clear(Instance* instance) {
  instance->objects.clear();
  instance->fds.clear();

  instance->read.Release();
  instance->write.Release();
}

base::Object Poll::get_read(Instance* instance) {
  if (instance->read.IsEmpty())
    instance->read = GetMatching(instance, POLLIN);
  return instance->read.GetObject();
}

base::Object Poll::get_write(Instance* instance) {
  if (instance->write.IsEmpty())
    instance->write = GetMatching(instance, POLLOUT);
  return instance->write.GetObject();
}

base::Object Poll::GetMatching(Instance* instance, int mask) {
  std::vector<base::Variant> values;

  for (std::vector<struct pollfd>::iterator iter = instance->fds.begin();
       iter != instance->fds.end();
       ++iter) {
    if ((iter->events & mask) != 0 && iter->revents != 0) {
      base::Object object(instance->objects.find(iter->fd)->second.GetObject());
      if (object.IsEmpty())
        values.push_back(base::Variant::Int32(iter->fd));
      else
        values.push_back(base::Variant::Object(object));
    }
  }

  return base::Array::FromVector(values);
}

}
}
