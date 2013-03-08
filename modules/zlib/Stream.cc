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
#include "modules/zlib/Stream.h"

#include <zlib.h>
#include <limits>

#include "modules/zlib/Error.h"
#include "utilities/Anchor.h"

namespace modules {
namespace zlib {

class Stream::Instance : public api::Class::Instance<Stream> {
 public:
  Instance(int level);
  ~Instance();

  bool is_inflate() { return level == std::numeric_limits<int>::min(); }
  bool is_deflate() { return level != std::numeric_limits<int>::min(); }

  void close();

  z_stream impl;
  int level;
  bool closed;

  std::string buffered;
};

namespace {

void* zalloc(void* opaque, uInt items, uInt size) {
  size_t nbytes = items * size;
  v8::V8::AdjustAmountOfExternalAllocatedMemory(static_cast<intptr_t>(nbytes));
  size_t* memory = static_cast<size_t*>(malloc(sizeof nbytes + nbytes));
  memory[0] = nbytes;
  return &memory[1];
}

void zfree(void*, void* address) {
  size_t* memory = static_cast<size_t*>(address);
  size_t nbytes = memory[-1];
  v8::V8::AdjustAmountOfExternalAllocatedMemory(-static_cast<intptr_t>(nbytes));
  return free(&memory[-1]);
}

}

Stream::Instance::Instance(int level)
    : level(level)
    , closed(false) {
  impl.zalloc = zalloc;
  impl.zfree = zfree;
  impl.opaque = Z_NULL;

  if (is_inflate())
    inflateInit(&impl);
  else
    deflateInit(&impl, level);
}

Stream::Instance::~Instance() {
  close();
}

void Stream::Instance::close() {
  if (!closed) {
    if (is_inflate())
      inflateEnd(&impl);
    else
      deflateEnd(&impl);

    closed = true;
  }
}

Stream::Stream()
    : api::Class("Stream", &constructor) {
  AddMethod<Stream>("write", write);
  AddMethod<Stream>("flush", flush);
  AddMethod<Stream>("close", close);
  AddMethod<Stream>("read", read);
}

builtin::Bytes::Instance* Stream::Deflate(builtin::Bytes::Instance* data,
                                          int level) {
  utilities::Anchor<Instance> instance(new Instance(level));

  write(instance, data);
  close(instance);

  return read(instance);
}

builtin::Bytes::Instance* Stream::Inflate(builtin::Bytes::Instance* data) {
  utilities::Anchor<Instance> instance(
      new Instance(std::numeric_limits<int>::min()));

  write(instance, data);
  close(instance);

  return read(instance);
}

Stream::Instance* Stream::constructor(Stream*, std::string type,
                                      utilities::Options options) {
  if (type != "inflate" && type != "deflate")
    throw base::SyntaxError("Stream type must be 'inflate' or 'deflate'");

  int level;

  if (type == "inflate")
    level = std::numeric_limits<int>::min();
  else {
    level = options.GetInt32("level", -1);
    if (level < -1 || level > 9)
      throw base::RangeError("Invalid argument: -1 <= level <= 9 required");
  }

  return new Instance(level);
}

namespace {

void process(Stream::Instance* instance, std::string data, int flush) {
  char output[16384];
  int ret;

  instance->impl.next_in =
      reinterpret_cast<decltype(instance->impl.next_in)>(
          const_cast<char*>(data.c_str()));
  instance->impl.avail_in = data.length();

  do {
    instance->impl.next_out =
        reinterpret_cast<decltype(instance->impl.next_out)>(output);
    instance->impl.avail_out = sizeof output;

    if (instance->is_inflate())
      ret = inflate(&instance->impl, flush);
    else
      ret = deflate(&instance->impl, flush);

    if (ret == Z_OK || ret == Z_STREAM_END)
      instance->buffered += std::string(
          output, sizeof output - instance->impl.avail_out);
  } while (ret == Z_OK);

  switch (ret) {
    case Z_STREAM_ERROR:
      throw Error("Stream error");
    case Z_DATA_ERROR:
      throw Error("Data error");
    case Z_MEM_ERROR:
      throw Error("Memory error");
  }
}

}

void Stream::write(Instance* instance, builtin::Bytes::Instance* bytes) {
  if (instance->closed)
    throw Error("Stream is closed");

  process(instance, builtin::Bytes::data(bytes), Z_NO_FLUSH);
}

void Stream::flush(Instance* instance) {
  if (instance->closed)
    throw Error("Stream is closed");

  process(instance, "", Z_SYNC_FLUSH);
}

void Stream::close(Instance* instance) {
  if (!instance->closed) {
    process(instance, "", Z_FINISH);
    instance->close();
  }
}

builtin::Bytes::Instance* Stream::read(Instance* instance) {
  if (instance->buffered.length() == 0)
    return NULL;

  builtin::Bytes::Instance* bytes =
      builtin::Bytes::FromContext()->New(instance->buffered);

  instance->buffered = "";

  return bytes;
}

}
}
