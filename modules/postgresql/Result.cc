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
#include "modules/postgresql/Result.h"

#include "modules/PostgreSQL.h"
#include "modules/postgresql/Error.h"
#include "modules/postgresql/Types.h"
#include "modules/postgresql/Utilities.h"
#include "utilities/Anchor.h"
#include "utilities/Shared.h"

namespace modules {
namespace postgresql {

class Result::Instance : public api::Class::Instance<Result> {
 public:
  Instance(PGconn* connection, PGresult* result, Types* types)
      : connection_(connection)
      , result_(new AccountedPGresult(result))
      , types_(types) {
    rows.resize(PQntuples(result));
  }

  PGconn* connection() { return connection_; }
  PGresult* result() { return result_->result(); }
  AccountedPGresult* accounted_result() { return result_; }
  Types* types() { return types_; }

  std::vector<base::Object::Persistent> rows;

  void Discard() {
    result_.Discard();
    rows.resize(0);
  }

 private:
  utilities::Shared<PGconn> connection_;
  utilities::Shared<AccountedPGresult> result_;
  utilities::Shared<Types> types_;
};

Result::Result()
    : api::Class("Result", this) {
  AddMethod<Result>("apply", &apply);
  AddMethod<Result>("close", &close);
  AddProperty<Result>("length", &get_length);
  AddIndexedHandler<Result>(&query, &get);
}

Result::Instance* Result::New(PGconn* connection, PGresult* result,
                              Types* types) {
  Instance* instance = new Instance(connection, result, types);
  ConstructObject<Result>(instance);
  return instance;
}

base::Object Result::GetObject(Instance* instance) {
  return instance->GetObject();
}

Result* Result::FromContext(v8::Handle<v8::Context> context) {
  return PostgreSQL::FromContext(context)->result();
}

std::vector<base::Variant> Result::apply(Result::Instance* instance,
                                         base::Function callback) {
  if (!instance->result())
    throw base::TypeError("access to closed result-set");

  return ApplyResult(instance->connection(), instance->result(),
                     instance->types(), callback);
}

void Result::close(Result::Instance* instance) {
  instance->Discard();
}

unsigned Result::get_length(Result::Instance* instance) {
  if (!instance->result())
    throw base::TypeError("access to closed result-set");
  return PQntuples(instance->result());
}

Row::Instance* Result::get(Result::Instance* instance, std::uint32_t index) {
  if (!instance->result())
    throw base::TypeError("access to closed result-set");
  if (index >= static_cast<decltype(index)>(PQntuples(instance->result())))
    return NULL;
  Row* cls = PostgreSQL::FromContext()->row();
  Row::Instance* row;
  if (instance->rows[index].GetObject().IsEmpty()) {
    row = cls->New(instance->connection(), instance->accounted_result(), index,
                   instance->types());
    instance->rows[index] = Row::GetObject(row);
  } else {
    row = instance->rows[index].GetObject().AsInstance(cls);
  }
  return row;
}

unsigned Result::query(Result::Instance* instance, std::uint32_t index) {
  if (!instance->result())
    throw base::TypeError("access to closed result-set");
  if (index >= static_cast<decltype(index)>(PQntuples(instance->result())))
    return base::PropertyFlags::kNotFound;
  return base::PropertyFlags::kEnumerable;
}

}
}

namespace conversions {

using namespace modules::postgresql;

template <>
Result::Instance* as_value(
    const base::Variant& value, Result::Instance**) {
  Result* result = Result::FromContext();
  if (!value.IsObject() || !result->HasInstance(value.AsObject()))
    throw base::TypeError("invalid argument, expected Result object");
  return Result::Instance::FromObject(result, value.AsObject());
}

template <>
base::Variant as_result(Result::Instance* result) {
  if (!result)
    return v8::Null();
  return result->GetObject().handle();
}

}

#endif // POSTGRESQL_SUPPORT
