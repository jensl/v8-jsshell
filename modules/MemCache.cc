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
#include "modules/MemCache.h"

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/memcache/Connection.h"

namespace modules {

MemCache::MemCache(const Features& features)
    : api::Module(kMemCache, "MemCache")
    , connection_(new memcache::Connection) {
}

MemCache::~MemCache() {
  delete connection_;
}

void MemCache::ExtendObject(base::Object target) {
  connection_->AddTo(target);
}

void MemCache::ExtendRuntime(api::Runtime& runtime) {
  AddTo(runtime.GetGlobalObject());
  ExtendObject(GetObject());
}

MemCache* MemCache::FromContext(v8::Handle<v8::Context> context) {
  return api::Module::FromContext<MemCache>(kMemCache, context);
}

} // modules

#endif // MEMCACHE_SUPPORT
