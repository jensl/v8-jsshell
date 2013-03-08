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

#ifndef MODULES_IO_BUFFERED_H
#define MODULES_IO_BUFFERED_H

#include "api/Class.h"
#include "modules/builtin/Bytes.h"
#include "utilities/Options.h"

namespace modules {
namespace io {

class Buffered : public api::Class {
 public:
  class Instance;

  Buffered();

 private:
  static Instance* constructor(Buffered*, base::Variant source);

  static builtin::Bytes::Instance* read(Instance* instance,
                                        Optional<unsigned> nbytes);

  static base::Variant readln(Instance* instance);
};

}
}

#endif // MODULES_IO_BUFFERED_H
