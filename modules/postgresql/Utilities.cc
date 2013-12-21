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

#if POSTGRESQL_SUPPORT

#include "Base.h"
#include "modules/postgresql/Utilities.h"

#include "modules/postgresql/Error.h"
#include "modules/postgresql/Types.h"
#include "utilities/Anchor.h"

namespace modules {
namespace postgresql {

AccountedPGresult::AccountedPGresult(PGresult* result)
    : result_(result)
    , size_(0) {
  std::uint64_t rowsize = 0, varsize = 0;

  for (int field = 0, nfields = PQnfields(result); field < nfields; ++field) {
    int fieldsize = PQfsize(result, field);
    if (fieldsize > 0)
      rowsize += fieldsize;
    else
      for (int row = 0, nrows = PQntuples(result); row < nrows; ++row)
        varsize += PQgetlength(result, row, field);
  }

  size_ = PQntuples(result) * rowsize + varsize;

  v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(
      static_cast<intptr_t>(size_));
}

AccountedPGresult::~AccountedPGresult() {
  v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(
      -static_cast<intptr_t>(size_));
  PQclear(result_);
}

void EnsureTransaction(PGconn* connection) {
  PGTransactionStatusType transaction_status =
      PQtransactionStatus(connection);

  if (transaction_status == PQTRANS_IDLE) {
    utilities::Anchor<PGresult> result(PQexec(connection, "BEGIN"));
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
      throw Error("failed to start transaction", result);
  }
}

double GetTimeValue(PGconn* connection, Types::PostgreSQLType type,
                    std::string string_value) {
  std::string query("SELECT EXTRACT(EPOCH FROM $1::");

  switch (type) {
    case Types::kDATE:
      query += "DATE";
      break;
    case Types::kTIMESTAMP:
      query += "TIMESTAMP";
      break;
    case Types::kTIMESTAMPTZ:
    default:
      query += "TIMESTAMP WITH TIME ZONE";
  }

  query += ")";

  const char* parameters[1] = { string_value.c_str() };

  utilities::Anchor<PGresult> result(
      PQexecParams(connection, query.c_str(), 1, 0, parameters, 0, 0, 0));

  if (PQresultStatus(result) != PGRES_TUPLES_OK)
    throw Error("failed to execute query", result);

  base::Variant value;
  value.SetString(PQgetvalue(result, 0, 0));
  return value.AsNumber() * 1000;
}

base::Variant GetField(PGconn* connection, PGresult* result, Types* types,
                       unsigned row, unsigned field) {
  if (PQgetisnull(result, row, field))
    return base::Variant::Null();

  std::string value(PQgetvalue(result, row, field));

  Types::PostgreSQLType type = types->TypeFromOid(PQftype(result, field));

  switch (type) {
    default:
      return base::Variant::String(value);

    case Types::kINTEGER:
    case Types::kBIGINT:
    case Types::kREAL:
    case Types::kDOUBLE_PRECISION:
      return base::Variant::Number(base::Variant::String(value).AsNumber());

    case Types::kBOOL:
      return base::Variant::Boolean(value == "t");

    case Types::kDATE:
    case Types::kTIMESTAMP:
    case Types::kTIMESTAMPTZ:
      return base::Variant(v8::Date::New(
          v8::Isolate::GetCurrent(), GetTimeValue(connection, type, value)));
  }
}

std::vector<base::Variant> ApplyResult(PGconn* connection, PGresult* result,
                                       Types* types, base::Function callback) {
  std::vector<base::Variant> values;

  for (unsigned row = 0, rows = PQntuples(result);
       row < rows;
       ++row) {
    v8::EscapableHandleScope handle_scope(v8::Isolate::GetCurrent());
    std::vector<base::Variant> arguments;

    for (unsigned field = 0, fields = PQnfields(result);
         field < fields;
         ++field)
      arguments.push_back(GetField(connection, result, types, row, field));

    values.push_back(handle_scope.Escape(
        callback.Call(base::Object(), arguments).handle()));
  }

  return values;
}

}
}

#endif // POSTGRESQL_SUPPORT
