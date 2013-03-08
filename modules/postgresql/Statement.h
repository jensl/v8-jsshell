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

#ifndef MODULES_POSTGRESQL_STATEMENT_H
#define MODULES_POSTGRESQL_STATEMENT_H

#if POSTGRESQL_SUPPORT

#include <libpq-fe.h>

#include "api/Class.h"
#include "modules/postgresql/Result.h"

namespace modules {
namespace postgresql {

class Types;

class Statement : public api::Class {
 public:
  class Instance;

  Statement();

  Instance* New(PGconn* connection, Types* types, std::string name,
                std::string query);

  static Statement* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Result::Instance* execute(Instance* instance,
                                   const std::vector<base::Variant>& rest);
  static std::vector<base::Variant> apply(
      Instance* instance, std::vector<std::vector<base::Variant> > values,
      Optional<base::Object> callback);
};

}
}

namespace conversions {

using namespace modules::postgresql;

template <>
Statement::Instance* as_value(
    const base::Variant& value, Statement::Instance**);

template <>
base::Variant as_result(Statement::Instance* result);

}

#endif // POSTGRESQL_SUPPORT
#endif // MODULES_POSTGRESQL_STATEMENT_H
