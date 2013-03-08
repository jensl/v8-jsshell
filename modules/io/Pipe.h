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

#ifndef MODULES_IO_PIPE_H
#define MODULES_IO_PIPE_H

#include "api/Class.h"
#include "modules/io/File.h"

namespace modules {
namespace io {

class Pipe : public api::Class {
 public:
  class Instance;

  Pipe();

  static Pipe* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Pipe*);

  static File::Instance* get_input(Instance* instance);
  static File::Instance* get_output(Instance* instance);
};

}
}

#endif // MODULES_IO_PIPE_H
