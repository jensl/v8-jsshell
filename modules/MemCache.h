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

#ifndef MODULES_MEMCACHE_H
#define MODULES_MEMCACHE_H

#if MEMCACHE_SUPPORT

#include "api/Module.h"
#include "utilities/Options.h"

namespace api {
class Runtime;
}

namespace modules {

class Features;

namespace memcache {
class Connection;
}

class MemCache : public api::Module {
 public:
  MemCache(const Features& features);
  virtual ~MemCache();

  virtual void ExtendObject(base::Object target) override;
  virtual void ExtendRuntime(api::Runtime& runtime) override;

  static MemCache* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  memcache::Connection* connection_;
};

} // modules

#endif // MEMCACHE_SUPPORT

#endif // MODULES_MEMCACHE_H
