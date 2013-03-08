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

#ifndef MODULES_OS_H
#define MODULES_OS_H

#include "api/Module.h"

namespace modules {

class Features;

namespace os {
class Process;
}

class OS : public api::Module {
 public:
  OS(const Features& features);
  ~OS();

  virtual void ExtendObject(base::Object target) override;
  virtual void ExtendRuntime(api::Runtime& runtime) override;

  os::Process* process() { return process_; }

  static OS* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  os::Process* process_;
};

}

#endif // MODULES_OS_H
