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

#if MEMCACHE_SUPPORT

#include "Base.h"
#include "modules/memcache/Connection.h"

#include <libmemcached/memcached.h>

#include "modules/MemCache.h"

#include "base/Error.h"
#include "utilities/Shared.h"

namespace modules {
namespace memcache {

class MemCacheError : public base::CustomError {
 public:
  explicit MemCacheError(std::string message)
      : CustomError("MemCache.Error") {
    set_message(message);
  }
};

class Connection::Instance : public api::Class::Instance<Connection> {
 public:
  Instance(memcached_st* connection)
      : connection(connection) {
  }

  virtual ~Instance() {
    memcached_free(connection);
  }

  utilities::Shared<memcached_st> connection;
};

Connection::Connection()
    : api::Class("Connection", &constructor) {
  AddMethod<Connection>("get", &get);
  AddMethod<Connection>("set", &set);
  AddMethod<Connection>("increment", &increment);
  AddMethod<Connection>("decrement", &decrement);
  AddMethod<Connection>("delete", &delete_key);
}

Connection::~Connection() {
}

Connection::Instance* Connection::constructor(
    Connection*, utilities::Options options) {
  std::string option_string;

  for (auto iter(options.begin()); iter != options.end(); ++iter) {
    if (iter != options.begin())
      option_string += " ";
    option_string += "--";
    option_string += iter->first;
    option_string += "=";
    option_string += iter->second.AsString();
  }

  memcached_st* connection = memcached(option_string.c_str(),
                                       option_string.length());

  if (!connection)
    throw MemCacheError("failed to establish memcache connection");

  Instance* instance = new Instance(connection);

  return instance;
}

/* static */
base::Object Connection::get(Connection::Instance* instance,
                             std::string key) {
  uint32_t flags = 0;
  memcached_return_t error;
  size_t value_length = 0;

  char* value = memcached_get(
      instance->connection, key.c_str(), key.length(), &value_length, &flags,
      &error);

  if (!value || error != MEMCACHED_SUCCESS)
    throw MemCacheError("failed to access memcache key");

  base::Object dict = base::Object::Create();
  std::string string_value(value, value_length);
  Optional<base::Variant> json_value = base::Variant::FromJSON(string_value);

  if (json_value.specified())
    dict.Put("value", json_value.value());
  dict.Put("string", string_value);
  dict.Put("flags", flags);
  free(value);

  return dict;
}

/* static */
void Connection::set(Connection::Instance* instance,
                     std::string key,
                     base::Variant value,
                     Optional<unsigned> flags) {
  std::string value_string = value.AsJSON();

  time_t expiration = 0;
  uint32_t flag_value = 0;
  if (flags.specified())
    flag_value = flags.value();

  memcached_return_t result = memcached_set(
      instance->connection, key.c_str(), key.length(), value_string.c_str(),
      value_string.length(), expiration, flag_value);

  if (result != MEMCACHED_SUCCESS)
    throw MemCacheError("Failed to set memcache value");
}

/* static */
double Connection::increment(Connection::Instance* instance,
                             std::string key) {
  uint32_t offset = 1;
  uint64_t value = 0;

  memcached_return_t result = memcached_increment(
      instance->connection, key.c_str(), key.length(), offset, &value);

  if (result != MEMCACHED_SUCCESS)
    throw MemCacheError("Failed to increment memcache value");

  return static_cast<double>(value);
}

/* static */
double Connection::decrement(Connection::Instance* instance,
                             std::string key) {
  uint32_t offset = 1;
  uint64_t value = 0;

  memcached_return_t result = memcached_decrement(
      instance->connection, key.c_str(), key.length(), offset, &value);

  if (result != MEMCACHED_SUCCESS)
    throw MemCacheError("Failed to decrement memcache value");

  return static_cast<double>(value);
}

/* static */
void Connection::delete_key(Connection::Instance* instance,
                            std::string key) {
  time_t expiration = 0;

  memcached_return_t result = memcached_delete(
      instance->connection, key.c_str(), key.length(), expiration);

  if (result != MEMCACHED_SUCCESS)
    throw MemCacheError("Failed to decrement memcache value");
}

} // memcache
} // modules

#endif // MEMCACHE_SUPPORT
