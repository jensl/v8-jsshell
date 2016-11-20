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
#include "modules/postgresql/Row.h"

#include "modules/PostgreSQL.h"
#include "modules/postgresql/Error.h"
#include "modules/postgresql/Result.h"
#include "modules/postgresql/Types.h"
#include "modules/postgresql/Utilities.h"
#include "utilities/Anchor.h"
#include "utilities/Shared.h"

namespace modules {
namespace postgresql {

class Row::Instance : public api::Class::Instance<Row> {
 public:
  Instance(PGconn* connection, AccountedPGresult* result, unsigned index, Types* types)
      : connection_(connection)
      , result_(result)
      , types_(types)
      , index_(index) {
  }

  PGconn* connection() { return connection_; }
  PGresult* result() { return result_->result(); }
  Types* types() { return types_; }
  unsigned index() { return index_; }

 private:
  utilities::Shared<PGconn> connection_;
  utilities::Shared<AccountedPGresult> result_;
  utilities::Shared<Types> types_;
  unsigned index_;
};

Row::Row()
    : api::Class("Row", this) {
  AddNamedHandler<Row>(&list, &query, &get);
  AddIndexedHandler<Row>(&query, &get);
}

Row::Instance* Row::New(PGconn* connection, AccountedPGresult* result,
                        int index, Types* types) {
  Instance* instance = new Instance(connection, result, index, types);
  ConstructObject<Row>(instance);
  return instance;
}

base::Object Row::GetObject(Instance* instance) {
  return instance->GetObject();
}

Row* Row::FromContext(v8::Handle<v8::Context> context) {
  return PostgreSQL::FromContext(context)->row();
}

std::vector<std::string> Row::list(Instance* instance) {
  std::vector<std::string> names;
  for (int index = 0, nfields = PQnfields(instance->result());
       index < nfields;
       ++index)
    names.push_back(PQfname(instance->result(), index));
  return names;
}

unsigned Row::query(Instance* instance, std::string name) {
  if (PQfnumber(instance->result(), name.c_str()) < 0)
    return base::PropertyFlags::kNotFound;
  return base::PropertyFlags::kNone;
}

base::Variant Row::get(Row::Instance* instance, std::string name) {
  int index = PQfnumber(instance->result(), name.c_str());
  if (index < 0)
    return base::Variant();
  return get(instance, index);
}

unsigned Row::query(Instance* instance, std::uint32_t index) {
  if (index >= static_cast<decltype(index)>(PQnfields(instance->result())))
    return base::PropertyFlags::kNotFound;
  return base::PropertyFlags::kNone;
}

base::Variant Row::get(Instance* instance, std::uint32_t index) {
  if (index >= static_cast<decltype(index)>(PQnfields(instance->result())))
    return base::Variant();
  return GetField(instance->connection(), instance->result(), instance->types(),
                  instance->index(), index);
}

}
}

namespace conversions {

using namespace modules::postgresql;

template <>
Row::Instance* as_value(const base::Variant& value, Row::Instance**) {
  Row* row = Row::FromContext();
  if (!value.IsObject() || !row->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected Row object");
  return Row::Instance::FromObject(row, value.AsObject());
}

template <>
base::Variant as_result(Row::Instance* result) {
  if (!result)
    return v8::Null(CurrentIsolate());
  return result->GetObject().handle();
}

}

#endif // POSTGRESQL_SUPPORT
