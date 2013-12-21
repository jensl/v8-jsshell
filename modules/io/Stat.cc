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
#include "modules/io/Stat.h"

#include "modules/IO.h"

namespace modules {
namespace io {

class Stat::Instance : public api::Class::Instance<Stat> {
 public:
  Instance(const struct stat& buf)
      : buf(buf) {
  }

  struct stat buf;
};

Stat::Stat()
    : api::Class("Stat", this) {
  AddMethod<Stat>("isDirectory", &is<S_IFDIR>);
  AddMethod<Stat>("isSocket", &is<S_IFSOCK>);
  AddMethod<Stat>("isSymbolicLink", &is<S_IFLNK>);
  AddMethod<Stat>("isRegularStat", &is<S_IFREG>);
  AddMethod<Stat>("isCharacterDevice", &is<S_IFCHR>);
  AddMethod<Stat>("isBlockDevice", &is<S_IFBLK>);
  AddMethod<Stat>("isNamedPipe", &is<S_IFIFO>);

  AddProperty<Stat>("dev", &get_dev);
  AddProperty<Stat>("ino", &get_ino);
  AddProperty<Stat>("mode", &get_mode);
  AddProperty<Stat>("nlink", &get_nlink);
  AddProperty<Stat>("uid", &get_uid);
  AddProperty<Stat>("gid", &get_gid);
  AddProperty<Stat>("rdev", &get_rdev);
  AddProperty<Stat>("size", &get_size);
  AddProperty<Stat>("blksize", &get_blksize);
  AddProperty<Stat>("blocks", &get_blocks);
  AddProperty<Stat>("atime", &get_atime);
  AddProperty<Stat>("mtime", &get_mtime);
  AddProperty<Stat>("ctime", &get_ctime);
}

Stat::Instance* Stat::New(const struct stat& buf) {
  Instance* instance = new Instance(buf);
  instance->CreateObject(this);
  return instance;
}

Stat* Stat::FromContext(v8::Handle<v8::Context> context) {
  return IO::FromContext(context)->stat();
}

template <unsigned Flag>
bool Stat::is(Instance* instance) {
  return (instance->buf.st_mode & Flag) != 0;
}

std::int64_t Stat::get_dev(Instance* instance) {
  return instance->buf.st_dev;
}

std::int64_t Stat::get_ino(Instance* instance) {
  return instance->buf.st_ino;
}

std::int64_t Stat::get_mode(Instance* instance) {
  return instance->buf.st_mode;
}

std::int64_t Stat::get_nlink(Instance* instance) {
  return instance->buf.st_nlink;
}

std::int64_t Stat::get_uid(Instance* instance) {
  return instance->buf.st_uid;
}

std::int64_t Stat::get_gid(Instance* instance) {
  return instance->buf.st_gid;
}

std::int64_t Stat::get_rdev(Instance* instance) {
  return instance->buf.st_gid;
}

std::int64_t Stat::get_size(Instance* instance) {
  return instance->buf.st_size;
}

std::int64_t Stat::get_blksize(Instance* instance) {
  return instance->buf.st_blksize;
}

std::int64_t Stat::get_blocks(Instance* instance) {
  return instance->buf.st_blocks;
}

namespace {

base::Object MakeDate(struct timespec spec) {
  double time = (static_cast<double>(spec.tv_sec) * 1000 +
                 static_cast<double>(spec.tv_nsec) / 1000000);
  return v8::Date::New(v8::Isolate::GetCurrent(), time).As<v8::Object>();
}

}

base::Object Stat::get_atime(Instance* instance) {
  return MakeDate(instance->buf.st_atim);
}

base::Object Stat::get_mtime(Instance* instance) {
  return MakeDate(instance->buf.st_mtim);
}

base::Object Stat::get_ctime(Instance* instance) {
  return MakeDate(instance->buf.st_ctim);
}

}
}

namespace conversions {

using namespace modules::io;

template <>
Stat::Instance* as_value(const base::Variant& value, Stat::Instance**) {
  Stat* stat = Stat::FromContext();
  if (!value.IsObject() || !stat->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected Stat object");
  return Stat::Instance::FromObject(stat, value.AsObject());
}

template <>
base::Variant as_result(Stat::Instance* result) {
  return result->GetObject().handle();
}

}
