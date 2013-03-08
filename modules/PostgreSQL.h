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

#ifndef MODULES_POSTGRESQL_H
#define MODULES_POSTGRESQL_H

#if POSTGRESQL_SUPPORT

#include "api/Module.h"

namespace api {
class Runtime;
}

namespace modules {

class Features;

namespace postgresql {
class Connection;
class Statement;
class Result;
class Row;
}

class PostgreSQL : public api::Module {
 public:
  PostgreSQL(const Features& features);
  ~PostgreSQL();

  virtual void ExtendObject(base::Object target) override;
  virtual void ExtendRuntime(api::Runtime& runtime) override;

  postgresql::Connection* connection() { return connection_; }
  postgresql::Statement* statement() { return statement_; }
  postgresql::Result* result() { return result_; }
  postgresql::Row* row() { return row_; }

  static PostgreSQL* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  postgresql::Connection* connection_;
  postgresql::Statement* statement_;
  postgresql::Result* result_;
  postgresql::Row* row_;
};

}

#endif // POSTGRESQL_SUPPORT
#endif // MODULES_POSTGRESQL_H
