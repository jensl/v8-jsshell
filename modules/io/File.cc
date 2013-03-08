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
#include "modules/io/File.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include <memory>

#include "api/GC.h"
#include "modules/io/IOError.h"
#include "modules/BuiltIn.h"
#include "modules/IO.h"
#include "utilities/Anchor.h"
#include "utilities/FileDescriptor.h"
#include "utilities/File.h"
#include "utilities/Path.h"

namespace modules {
namespace io {

class File::Instance : public Class::Instance<File> {
 public:
  Instance(int fd, std::string path, std::string mode);
  ~Instance();

  int fd;
  std::string path;
  std::string mode;
};

File::Instance::Instance(int fd, std::string path, std::string mode)
    : fd(fd)
    , path(path)
    , mode(mode) {
}

File::Instance::~Instance() {
  if (fd != -1) {
    if (api::GC::IsRunning())
      fprintf(::stderr, "WARNING: open file garbage collected: %s\n", path.c_str());

    /* Note: ignoring failure to close here; no practical way to
       signal it anyway. */
    ::close(fd);
  }
}

File::File()
    : Class("File", constructor) {
  AddMethod<File>("fileno", fileno);
  AddMethod<File>("setBlocking", setBlocking);
  AddMethod<File>("read", read);
  AddMethod<File>("write", write);
  AddMethod<File>("stat", stat);
  AddMethod<File>("close", close);

  AddProperty<File>("closed", get_closed);

  AddClassFunction<File>("read", read);
  AddClassFunction<File>("write", write);
  AddClassFunction<File>("fdopen", fdopen);
  AddClassFunction<File>("stat", stat);
  AddClassFunction<File>("dup", dup);
  AddClassFunction<File>("dup2", dup2);
  AddClassFunction<File>("link", link);
  AddClassFunction<File>("symlink", symlink);
  AddClassFunction<File>("unlink", unlink);
  AddClassFunction<File>("chmod", chmod);
  AddClassFunction<File>("rename", rename);
  AddClassFunction<File>("chdir", chdir);
  AddClassFunction<File>("mkdir", mkdir);
  AddClassFunction<File>("utimes", utimes);

  AddClassFunction<File>("listDirectory", listDirectory);

  AddClassFunction<File>("isDirectory", is<S_IFDIR>);
  AddClassFunction<File>("isSocket", is<S_IFSOCK>);
  AddClassFunction<File>("isSymbolicLink", is<S_IFLNK>);
  AddClassFunction<File>("isRegularFile", is<S_IFREG>);
  AddClassFunction<File>("isCharacterDevice", is<S_IFCHR>);
  AddClassFunction<File>("isBlockDevice", is<S_IFBLK>);
  AddClassFunction<File>("isNamedPipe", is<S_IFIFO>);

  AddClassProperty<File>("stdin", get_stdin);
  AddClassProperty<File>("stdout", get_stdout);
  AddClassProperty<File>("stderr", get_stderr);
}

int File::Steal(Instance* instance) {
  int fd = instance->fd;
  instance->fd = -1;
  return fd;
}

File::Instance* File::New(int fd, std::string mode) {
  Instance* instance = new Instance(fd, "<unknown>", mode);
  instance->CreateObject(this);
  return instance;
}

File::Instance* File::FromObject(base::Object object) {
  return Instance::FromObject(this, object);
}

File* File::FromContext(v8::Handle<v8::Context> context) {
  return IO::FromContext(context)->file();
}

File::Instance* File::NewInstance(std::string path, std::string mode_arg) {
  int flags;

  if (mode_arg == "r")
    flags = O_RDONLY;
  else if (mode_arg == "w" || mode_arg == "a")
    flags = O_WRONLY;
  else if (mode_arg == "r+" || mode_arg == "w+" || mode_arg == "a+")
    flags = O_RDWR;
  else
    throw base::SyntaxError("invalid mode argument");

  mode_t mode = 0666;

  if (mode_arg == "a" || mode_arg == "a+")
    flags |= O_APPEND;
  if (mode_arg == "w" || mode_arg == "w+")
    flags |= O_CREAT | O_TRUNC;

  int fd = open(path.c_str(), flags, mode);

  if (fd == -1)
    throw IOError("open() failed", errno) << " (" << path << ")";

  return new Instance(fd, path, mode_arg);
}

File::Instance* File::constructor(
    File*, std::string path, Optional<std::string> mode) {
  return NewInstance(path, mode.value("r"));
}

int File::fileno(Instance* instance) {
  return instance->fd;
}

void File::setBlocking(Instance* instance, bool value) {
  int flags = ::fcntl(instance->fd, F_GETFL);

  if (flags == -1)
    throw IOError("fnctl() failed", errno);

  int new_flags;

  if (!value)
    new_flags = flags | O_NONBLOCK;
  else
    new_flags = flags & ~O_NONBLOCK;

  if (new_flags != flags) {
    int retval = ::fcntl(instance->fd, F_SETFL, new_flags);

    if (retval == -1)
      throw IOError("fnctl() failed", errno);
  }
}

builtin::Bytes::Instance* File::read(Instance* instance,
                                     Optional<unsigned> buflen) {
  try {
    std::string data;

    if (buflen.specified()) {
      if (buflen.value() == 0)
        throw base::TypeError("invalid argument; zero buffer length");

      data = utilities::FileDescriptor::ReadSome(instance->fd, buflen.value());
    } else {
      data = utilities::FileDescriptor::ReadAll(instance->fd);
    }

    return builtin::Bytes::FromContext()->New(data);
  } catch (utilities::FileDescriptor::EndOfFile&) {
    return NULL;
  } catch (utilities::FileDescriptor::Error& error) {
    throw IOError("read() failed", error.errnum);
  }
}

void File::write(Instance* instance, builtin::Bytes::Instance* bytes) {
  unsigned remaining = builtin::Bytes::data(bytes).length();

  while (remaining) {
    ssize_t nwritten = ::write(instance->fd, builtin::Bytes::data(bytes).c_str(), remaining);

    if (nwritten == -1) {
      if (errno == EINTR)
        continue;
      else
        throw IOError("write() failed", errno);
    }

    remaining -= nwritten;
  }
}

builtin::Bytes::Instance* File::read(File*, std::string path) {
  utilities::Anchor<Instance> instance(NewInstance(path, "r"));
  std::string data;

  try {
    data = utilities::FileDescriptor::ReadAll(instance->fd);
  } catch (utilities::FileDescriptor::EndOfFile&) {
  } catch (utilities::FileDescriptor::Error& error) {
    throw IOError("read() failed", error.errnum);
  }

  return builtin::Bytes::FromContext()->New(data);
}

void File::write(File*, std::string path, builtin::Bytes::Instance* data) {
  utilities::Anchor<Instance> instance(NewInstance(path, "w"));

  write(instance, data);
  close(instance);
}


File::Instance* File::fdopen(File* module, utilities::FileDescriptor fd,
                             Optional<std::string> mode) {
  return module->New(fd, mode.value("r"));
}

Stat::Instance* File::stat(Instance* instance) {
  if (instance->fd == -1)
    throw base::TypeError("file not open");

  struct stat buf;

  if (::fstat(instance->fd, &buf) == -1)
    throw IOError("fstat() failed", errno);

  return Stat::FromContext()->New(buf);
}

void File::close(Instance* instance) {
  if (instance->fd == -1)
    throw base::TypeError("file not open");

  while (true) {
    if (::close(instance->fd) == -1) {
      if (errno == EINTR)
        continue;
      else
        throw IOError("close() failed", errno);
    } else {
      break;
    }
  }

  instance->fd = -1;
}

bool File::get_closed(Instance* instance) {
  return instance->fd == -1;
}

Stat::Instance* File::stat(File* module, std::string path) {
  struct stat buf;

  if (::stat(path.c_str(), &buf) == -1)
    throw IOError("stat() failed", errno);

  return Stat::FromContext()->New(buf);
}

int File::dup(File*, utilities::FileDescriptor oldfd) {
  int newfd = ::dup(oldfd);
  if (newfd == -1)
    throw IOError("dup() failed", errno);
  return newfd;
}

void File::dup2(File*, utilities::FileDescriptor oldfd, int newfd) {
  if (::dup2(oldfd, newfd) == -1)
    throw IOError("dup2() failed", errno);
}

void File::link(File*, std::string old_path, std::string new_path) {
  if (::link(old_path.c_str(), new_path.c_str()) == -1)
    throw IOError("link() failed", errno);
}

void File::symlink(File*, std::string old_path, std::string new_path) {
  if (::symlink(old_path.c_str(), new_path.c_str()) == -1)
    throw IOError("symlink() failed", errno);
}

void File::unlink(File*, std::string path) {
  if (::unlink(path.c_str()) == -1)
    throw IOError("unlink() failed", errno);
}

void File::chmod(File*, std::string path, int mode) {
  if (::chmod(path.c_str(), mode) == -1)
    throw IOError("chmod() failed", errno);
}

void File::rename(File*, std::string old_path, std::string new_path) {
  if (::rename(old_path.c_str(), new_path.c_str()) == -1)
    throw IOError("rename() failed", errno)
        << " (" << old_path << " => " << new_path << ")";
}

void File::chdir(File*, std::string path) {
  if (::chdir(path.c_str()) == -1)
    throw IOError("chdir() failed", errno) << " (" << path << ")";
}

void File::mkdir(File*, std::string path, Optional<int> mode_arg) {
  mode_t mode;
  if (mode_arg.specified())
    mode = static_cast<mode_t>(mode_arg.value());
  else
    mode = 0777;
  if (::mkdir(path.c_str(), mode) == -1)
    throw IOError("mkdir() failed", errno) << " (" << path << ")";
}

namespace {

void SetTimeval(struct timeval& tv, double time) {
  tv.tv_sec = static_cast<long>(time / 1000);
  tv.tv_usec = static_cast<long>((time - tv.tv_sec * 1000) * 1000);
}

}

void File::utimes(File*, std::string path, Optional<double> atime_opt,
                  Optional<double> mtime_opt) {
  struct timeval times[2], *timesp = NULL;

  if (atime_opt.specified()) {
    SetTimeval(times[0], atime_opt.value());
    if (mtime_opt.specified())
      SetTimeval(times[1], mtime_opt.value());
    else
      times[1] = times[0];
    timesp = times;
  }

  if (::utimes(path.c_str(), timesp) != 0)
    throw IOError("utimes() failed", errno) << " (" << path << ")";
}

base::Object File::listDirectory(File* module, std::string path){
  DIR* dir = ::opendir(path.c_str());

  if (!dir)
    throw IOError("opendir() failed", errno) << ": " << path;

  std::vector<std::string> entries;
  struct dirent* entry;

  while ((entry = ::readdir(dir)) != NULL)
    entries.push_back(entry->d_name);

  ::closedir(dir);

  return base::Array::FromVector(entries);
}

template <unsigned Flag>
bool File::is(File* module, std::string path) {
  struct stat buf;

  if (::stat(path.c_str(), &buf) == -1) {
    if (errno == ENOENT || errno == ENOTDIR)
      return false;
    throw IOError("stat() failed", errno);
  }

  return (buf.st_mode & Flag) != 0;
}

File::Instance* File::get_stdin(File* cls) {
  if (cls->stdin.IsEmpty()) {
    Instance* instance = cls->New(0, "r");
    cls->stdin = instance->GetObject();
  }

  return Instance::FromObject(cls, cls->stdin.GetObject());
}

File::Instance* File::get_stdout(File* cls) {
  if (cls->stdout.IsEmpty()) {
    Instance* instance = cls->New(1, "a");
    cls->stdout = instance->GetObject();
  }

  return Instance::FromObject(cls, cls->stdout.GetObject());
}

File::Instance* File::get_stderr(File* cls) {
  if (cls->stderr.IsEmpty()) {
    Instance* instance = cls->New(2, "a");
    cls->stderr = instance->GetObject();
  }

  return Instance::FromObject(cls, cls->stderr.GetObject());
}

}
}

namespace conversions {

using namespace modules::io;

template <>
File::Instance* as_value(const base::Variant& value, File::Instance**) {
  File* file = File::FromContext();
  if (!value.IsObject() || !file->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected File object");
  return File::Instance::FromObject(file, value.AsObject());
}

template <>
base::Variant as_result(File::Instance* result) {
  return result->GetObject().handle();
}

}
