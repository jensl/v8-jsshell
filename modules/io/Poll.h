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

#ifndef MODULES_IO_POLL_H
#define MODULES_IO_POLL_H

#include "api/Class.h"

namespace modules {
namespace io {

class Poll : public api::Class {
 public:
  class Instance;

  Poll();

  static Poll* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Poll*);

  static void register_(Instance* instance, base::Variant file,
                        Optional<std::string> mode);
  static void unregister(Instance* instance, base::Variant file);
  static bool poll(Instance* instance, Optional<int> timeout);
  static bool isEmpty(Instance* instance);
  static void clear(Instance* instance);

  static base::Object get_read(Instance* instance);
  static base::Object get_write(Instance* instance);

  static base::Object GetMatching(Instance* instance, int mask);
};

}
}

#endif // MODULES_IO_POLL_H
