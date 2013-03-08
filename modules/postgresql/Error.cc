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

#include "Base.h"
#include "modules/postgresql/Error.h"

#include <string.h>
#include <stdio.h>

#include <memory>

namespace modules {
namespace postgresql {

Error::Error(std::string message)
    : CustomError("PostgreSQL.Error") {
  set_message(message);
}

Error::Error(std::string fallback, PGconn* connection)
    : CustomError("PostgreSQL.Error") {
  std::string message;
  if (connection)
    message = PQerrorMessage(connection);
  else
    message = fallback;
  set_message(message);
}

Error::Error(std::string fallback, PGresult* result)
    : CustomError("PostgreSQL.Error") {
  std::string message;
  if (result)
    message = PQresultErrorMessage(result);
  else
    message = fallback;
  set_message(message);
}

}
}
