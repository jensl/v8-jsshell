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
#include "modules/postgresql/Statement.h"

#include <libpq-fe.h>

#include "modules/PostgreSQL.h"
#include "modules/postgresql/Error.h"
#include "modules/postgresql/Types.h"
#include "modules/postgresql/Formatter.h"
#include "modules/postgresql/Utilities.h"

#include "utilities/Anchor.h"
#include "utilities/Shared.h"

namespace modules {
namespace postgresql {

class Statement::Instance : public api::Class::Instance<Statement> {
 public:
  Instance(PGconn* connection, Types* types, std::string name,
           std::string query)
      : connection(connection)
      , types(types)
      , name(name)
      , query(query) {
  }

  utilities::Shared<PGconn> connection;
  utilities::Shared<Types> types;
  std::string name;
  std::string query;
};

Statement::Statement()
    : api::Class("Statement", this) {
  AddMethod<Statement>("execute", &execute);
  AddMethod<Statement>("apply", &apply);
}

Statement::Instance* Statement::New(PGconn* connection, Types* types,
                                    std::string name, std::string query) {
  Instance* instance = new Instance(connection, types, name, query);
  instance->CreateObject(this);
  return instance;
}

Statement* Statement::FromContext(v8::Handle<v8::Context> context) {
  return PostgreSQL::FromContext(context)->statement();
}

Result::Instance* Statement::execute(Instance* instance,
                                     const std::vector<base::Variant>& rest) {
  Formatter formatter;

  formatter.Format(instance->query, rest);

  const std::vector<const char*>& parameters(formatter.parameters());
  const std::vector<int>& lengths(formatter.lengths());
  const std::vector<int>& formats(formatter.formats());

  EnsureTransaction(instance->connection);

  utilities::Anchor<PGresult> result(
      PQexecPrepared(instance->connection, instance->name.c_str(),
                     parameters.size(), parameters.data(), lengths.data(),
                     formats.data(), 0));

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

std::vector<base::Variant> Statement::apply(
    Instance* instance, std::vector<std::vector<base::Variant>> values,
    Optional<base::Object> callback) {
  std::vector<std::vector<base::Variant> >::const_iterator iter(values.begin());
  std::vector<base::Variant> all_return_values;

  while (iter != values.end()) {
    Formatter formatter;

    formatter.Format(instance->query, *iter);

    const std::vector<const char*>& parameters(formatter.parameters());
    const std::vector<int>& lengths(formatter.lengths());
    const std::vector<int>& formats(formatter.formats());

    if (iter == values.begin())
      EnsureTransaction(instance->connection);

    utilities::Anchor<PGresult> result(
        PQexecPrepared(instance->connection, instance->name.c_str(),
                       parameters.size(), parameters.data(), lengths.data(),
                       formats.data(), 0));

    switch (PQresultStatus(result)) {
      case PGRES_COMMAND_OK:
        break;
      case PGRES_TUPLES_OK:
        if (callback.specified()) {
          std::vector<base::Variant> return_values(
              ApplyResult(instance->connection, result, instance->types,
                          callback.value()));

          all_return_values.insert(all_return_values.end(),
                                   return_values.begin(), return_values.end());
        }
        break;
      default:
        throw Error("failed to execute query", result);
    }

    ++iter;
  }

  return all_return_values;
}

}
}

namespace conversions {

using namespace modules::postgresql;

template <>
Statement::Instance* as_value(
    const base::Variant& value, Statement::Instance**) {
  Statement* statement = Statement::FromContext();
  if (!value.IsObject() || !statement->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected Statement object");
  return Statement::Instance::FromObject(statement, value.AsObject());
}

template <>
base::Variant as_result(Statement::Instance* result) {
  if (!result)
    return v8::Null();
  return result->GetObject().handle();
}

}

#endif // POSTGRESQL_SUPPORT
