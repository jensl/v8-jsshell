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

#ifndef MODULES_MODULES_H
#define MODULES_MODULES_H

#include <map>
#include <string>

namespace modules {

enum {
  kModuleObject,

  kBuiltIn,
  kIO,
  kOS,
#if LIBCURL_SUPPORT
  kURL,
#endif
#if POSTGRESQL_SUPPORT
  kPostgreSQL,
#endif
#if ZLIB_SUPPORT
  kZLib,
#endif
#if MEMCACHE_SUPPORT
  kMemCache,
#endif

  kTesting
};

class Features {
 public:
  Features(const std::map<std::string, bool>& features);

  bool Enabled(std::string feature) const;
  bool Disabled(std::string feature) const;

  typedef std::map<std::string, bool>::const_iterator const_iterator;

  const_iterator begin() const;
  const_iterator end() const;

 private:
  const std::map<std::string, bool>& features_;

  Features(const Features& features);
};


}

#endif // MODULES_MODULES_H
