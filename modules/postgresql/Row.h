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

#ifndef MODULES_POSTGRESQL_ROW_H
#define MODULES_POSTGRESQL_ROW_H

#if POSTGRESQL_SUPPORT

#include <libpq-fe.h>

#include "api/Class.h"

namespace modules {
namespace postgresql {

class AccountedPGresult;
class Types;

class Row : public api::Class {
 public:
  class Instance;

  Row();

  Instance* New(PGconn* connection, AccountedPGresult* result, int index,
                Types* types);

  static base::Object GetObject(Instance* instance);

  static Row* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Row*);

  static std::vector<std::string> list(Instance* instance);
  static unsigned query(Instance* instance, std::string name);
  static base::Variant get(Instance* instance, std::string name);

  static unsigned query(Instance* instance, std::uint32_t index);
  static base::Variant get(Instance* instance, std::uint32_t index);
};

}
}

namespace conversions {

using namespace modules::postgresql;

template <>
Row::Instance* as_value(const base::Variant& value, Row::Instance**);

template <>
base::Variant as_result(Row::Instance* result);

}

#endif // POSTGRESQL_SUPPORT
#endif // MODULES_POSTGRESQL_ROW_H
