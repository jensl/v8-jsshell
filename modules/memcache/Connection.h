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

#ifndef MODULES_MEMCACHE_CONNECTION_H
#define MODULES_MEMCACHE_CONNECTION_H

#include "api/Class.h"
#include "utilities/Options.h"

namespace modules {
namespace memcache {

class Connection : public api::Class {
 public:
  class Instance;

  Connection();
  virtual ~Connection();

  static Connection* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Connection*, utilities::Options options);

  static base::Object get(Instance* instance, std::string key);
  static void set(Instance* instance, std::string key, base::Variant value,
                  Optional<unsigned> flags);
  static double increment(Instance* instance, std::string key);
  static double decrement(Instance* instance, std::string key);
  static void delete_key(Instance* instance, std::string key);
};

} // memcache
} // modules

#endif // MODULES_MEMCACHE_CONNECTION_H



