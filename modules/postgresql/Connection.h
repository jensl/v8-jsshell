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

#ifndef MODULES_POSTGRESQL_CONNECTION_H
#define MODULES_POSTGRESQL_CONNECTION_H

#if POSTGRESQL_SUPPORT

#include <libpq-fe.h>

#include "api/Class.h"
#include "modules/postgresql/Types.h"
#include "modules/postgresql/Result.h"
#include "modules/postgresql/Statement.h"
#include "utilities/Options.h"

namespace modules {
namespace postgresql {

class Connection : public api::Class {
 public:
  class Instance;

  Connection();
  ~Connection();

  static Types::PostgreSQLType TypeFromOid(Instance* instance, Oid oid);

 private:
  static Instance* constructor(Connection*, utilities::Options options);

  static Result::Instance* execute(Instance* instance, std::string query,
                                   const std::vector<base::Variant>& rest);
  static Statement::Instance* prepare(Instance* instance, std::string name,
                                      std::string query);

  static void commit(Instance* instance);
  static void rollback(Instance* instance);
  static void close(Instance* instance);

  static Types* FindTypeOids(PGconn* connection);
};

}
}

#endif // POSTGRESQL_SUPPORT
#endif // MODULES_POSTGRESQL_CONNECTION_H
