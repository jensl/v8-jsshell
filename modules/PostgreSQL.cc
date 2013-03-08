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
#include "modules/PostgreSQL.h"

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/postgresql/Connection.h"
#include "modules/postgresql/Statement.h"
#include "modules/postgresql/Result.h"
#include "modules/postgresql/Row.h"

namespace modules {

PostgreSQL::PostgreSQL(const Features& features)
    : api::Module(kPostgreSQL, "PostgreSQL")
    , connection_(new postgresql::Connection)
    , statement_(new postgresql::Statement)
    , result_(new postgresql::Result)
    , row_(new postgresql::Row) {
}

PostgreSQL::~PostgreSQL() {
  delete row_;
  delete result_;
  delete statement_;
  delete connection_;
}

void PostgreSQL::ExtendObject(base::Object target) {
  connection_->AddTo(target);
  statement_->AddTo(target);
  result_->AddTo(target);
  row_->AddTo(target);
}

void PostgreSQL::ExtendRuntime(api::Runtime& runtime) {
  AddTo(runtime.GetGlobalObject());
  ExtendObject(GetObject());
}

PostgreSQL* PostgreSQL::FromContext(v8::Handle<v8::Context> context) {
  return api::Module::FromContext<PostgreSQL>(kPostgreSQL, context);
}

}

#endif // POSTGRESQL_SUPPORT
