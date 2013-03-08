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
#include "modules/postgresql/Connection.h"

#include <libpq-fe.h>

#include "modules/PostgreSQL.h"
#include "modules/postgresql/Error.h"
#include "modules/postgresql/Formatter.h"
#include "modules/postgresql/Utilities.h"

#include "utilities/Anchor.h"
#include "utilities/Shared.h"

namespace modules {
namespace postgresql {

class Connection::Instance : public api::Class::Instance<Connection> {
 public:
  Instance(PGconn* connection, Types* types)
      : connection(connection)
      , types(types) {
  }

  utilities::Shared<PGconn> connection;
  utilities::Shared<Types> types;
};

Connection::Connection()
    : api::Class("Connection", &constructor) {
  AddMethod<Connection>("execute", &execute);
  AddMethod<Connection>("prepare", &prepare);
  AddMethod<Connection>("commit", &commit);
  AddMethod<Connection>("rollback", &rollback);
  AddMethod<Connection>("close", &close);
}

Connection::~Connection() {
}

namespace {

std::string escape(std::string value) {
  std::string::size_type index = value.find_first_of(" '\\");

  if (index == std::string::npos)
    return value;

  std::string escaped("'");
  std::string::size_type previous = 0;

  do {
    if (previous < index) {
      escaped.append(std::string(value, previous, index - previous));
      previous = index;
    }

    if (value[index] != ' ')
      escaped += '\\';

    index = value.find_first_of("'\\", index + 1);
  } while (index != std::string::npos);

  if (previous < index)
    escaped.append(std::string(value, previous));

  escaped += '\'';

  return escaped;
}

}

Connection::Instance* Connection::constructor(
    Connection*, utilities::Options options) {
#if POSTGRESQL_MAJOR >= 9
  std::vector<const char*> option_names;
  std::vector<const char*> option_values;

  for (auto iter(options.begin()); iter != options.end(); ++iter) {
    option_names.push_back(iter->first.c_str());
    option_values.push_back(iter->second.AsString().c_str());
  }

  option_names.push_back(NULL);
  option_values.push_back(NULL);

  PGconn* connection = PQconnectdbParams(
      option_names.data(), option_values.data(), 0);
#else
  std::string conninfo;

  for (auto iter(options.begin()); iter != options.end(); ++iter) {
    if (iter != options.begin())
      conninfo += " ";
    conninfo += iter->first;
    conninfo += "=";
    conninfo += escape(iter->second.AsString());
  }

  PGconn* connection = PQconnectdb(conninfo.c_str());
#endif

  if (!connection || PQstatus(connection) != CONNECTION_OK)
    throw Error("failed to establish database connection", connection);

  Instance* instance = new Instance(connection, FindTypeOids(connection));

  return instance;
}

Result::Instance* Connection::execute(Connection::Instance* instance,
                                      std::string query,
                                      const std::vector<base::Variant>& rest) {
  Formatter formatter;

  query = formatter.Format(query, rest);

  const std::vector<const char*>& parameters(formatter.parameters());
  const std::vector<int>& lengths(formatter.lengths());
  const std::vector<int>& formats(formatter.formats());

  EnsureTransaction(instance->connection);

  utilities::Anchor<PGresult> result(
      PQexecParams(instance->connection, query.c_str(), parameters.size(), 0,
                   parameters.data(), lengths.data(), formats.data(), 0));

  switch (PQresultStatus(result)) {
    case PGRES_COMMAND_OK:
      return NULL;
    case PGRES_TUPLES_OK:
      break;
    default:
      throw Error("failed to execute query", result);
  }

  return PostgreSQL::FromContext()->result()->New(
      instance->connection, result.Release(), instance->types);
}

Statement::Instance* Connection::prepare(Instance* instance, std::string name,
                                         std::string query) {
  PrepareFormatter formatter;

  std::string formatted = formatter.Format(query, std::vector<base::Variant>());

  utilities::Anchor<PGresult> result(
      PQprepare(instance->connection, name.c_str(), formatted.c_str(), 0, 0));

  if (PQresultStatus(result) != PGRES_COMMAND_OK)
    throw Error("failed to prepare query", result);

  return PostgreSQL::FromContext()->statement()->New(
      instance->connection, instance->types, name, query);
}

void Connection::commit(Connection::Instance* instance) {
  PGTransactionStatusType transaction_status =
      PQtransactionStatus(instance->connection);

  if (transaction_status == PQTRANS_INTRANS) {
    utilities::Anchor<PGresult> result(PQexec(instance->connection, "COMMIT"));
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
      throw Error("failed to commit transaction", result);
  }
}

void Connection::rollback(Connection::Instance* instance) {
  PGTransactionStatusType transaction_status =
      PQtransactionStatus(instance->connection);

  if (transaction_status == PQTRANS_INTRANS) {
    utilities::Anchor<PGresult> result(PQexec(instance->connection, "ROLLBACK"));
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
      throw Error("failed to roll transaction back", result);
  }
}

void Connection::close(Connection::Instance* instance) {
  if (instance->connection)
    instance->connection.Discard();
}

Types* Connection::FindTypeOids(PGconn* connection) {
  Types* types = new Types;

  for (int index = 0; index < Types::kCount; ++index)
    types->oids_[index] = InvalidOid;

  utilities::Anchor<PGresult> result(PQprepare(
      connection, "",
      "SELECT $1::bool, $2::smallint, $3::integer, $4::bigint, $5::real, "
      "$6::double precision, $7::char, $8::varchar, $9::text, $10::date, "
      "$11::timestamp, $12::timestamptz", 0, 0));

  if (PQresultStatus(result) != PGRES_COMMAND_OK)
    return NULL;

  result = PQdescribePrepared(connection, "");

  if (PQresultStatus(result) != PGRES_COMMAND_OK)
    return NULL;

  for (int index = 0; index < Types::kCount; ++index)
    types->oids_[index] = PQparamtype(result, index);

  return types;
}

}
}

#endif // POSTGRESQL_SUPPORT
