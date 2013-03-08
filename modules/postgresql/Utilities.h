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

#ifndef MODULES_POSTGRESQL_UTILITIES_H
#define MODULES_POSTGRESQL_UTILITIES_H

#if POSTGRESQL_SUPPORT

#include <libpq-fe.h>

#include "modules/postgresql/Types.h"
#include "utilities/Delete.h"

namespace utilities {

template <>
class Delete<PGresult> {
 public:
  void operator() (PGresult* result) {
    PQclear(result);
  }
};

template <>
class Delete<PGconn> {
 public:
  void operator() (PGconn* connection) {
    PQfinish(connection);
  }
};

}

namespace modules {
namespace postgresql {

class AccountedPGresult {
 public:
  AccountedPGresult(PGresult* result);
  ~AccountedPGresult();

  PGresult* result() { return result_; }

 private:
  PGresult* result_;
  std::uint64_t size_;
};

extern void EnsureTransaction(PGconn* connection);

/** Convert an SQL DATE, TIMESTAMP or TIMESTAMP WITH TIMEZONE value
    into an ECMAScript time value (milliseconds since epoch.) */
double GetTimeValue(PGconn* connection, Types::PostgreSQLType type,
                    std::string value);

/** Extract value of the N:th field of the N:th row in the result set. */
extern base::Variant GetField(PGconn* connection, PGresult* result,
                              Types* types, unsigned row, unsigned field);

/** Call the callback once for every row in the result set, with one
    argument per field in the row. */
extern std::vector<base::Variant> ApplyResult(
    PGconn* connection, PGresult* result, Types* types,
    base::Function callback);

}
}

#endif // POSTGRESQL_SUPPORT
#endif // MODULES_POSTGRESQL_RESULT_H
