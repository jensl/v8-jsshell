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

#include "modules/postgresql/Types.h"

#include <libpq-fe.h>

namespace modules {
namespace postgresql {

Types::PostgreSQLType Types::TypeFromOid(Oid oid) {
  for (int index = 0; index < Types::kCount; ++index)
    if (oids_[index] == oid)
      return static_cast<Types::PostgreSQLType>(index);
  return Types::kUnidentified;
}

}
}

#endif // POSTGRESQL_SUPPORT
