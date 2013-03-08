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

#ifndef MODULES_POSTGRESQL_TYPES_H
#define MODULES_POSTGRESQL_TYPES_H

#if POSTGRESQL_SUPPORT

#include <libpq-fe.h>

namespace modules {
namespace postgresql {

class Connection;

class Types {
 public:
  enum PostgreSQLType {
    kUnidentified = -1,

    kBOOL,
    kSMALLINT,
    kINTEGER,
    kBIGINT,
    kREAL,
    kDOUBLE_PRECISION,
    kCHAR,
    kVARCHAR,
    kTEXT,
    kDATE,
    kTIMESTAMP,
    kTIMESTAMPTZ,

    kCount
  };

  Types::PostgreSQLType TypeFromOid(Oid oid);

 private:
  friend class Connection;

  Oid oids_[static_cast<int>(Types::kCount)];
};

}
}

#endif // POSTGRESQL_SUPPORT
#endif // MODULES_POSTGRESQL_TYPES_H
