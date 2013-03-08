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

#ifndef MODULES_BUILTIN_H
#define MODULES_BUILTIN_H

#include "api/Module.h"

namespace modules {

namespace builtin {
class Bytes;
class Module;
}

class BuiltIn : public api::Module {
 public:
  BuiltIn();
  ~BuiltIn();

  virtual void ExtendObject(base::Object target) override;

  builtin::Bytes* bytes() { return bytes_; }
  builtin::Module* module() { return module_; }

  static std::string format(BuiltIn* module, std::string format,
                            const std::vector<base::Variant>& rest);

  static void write(BuiltIn* module, std::string format,
                    const std::vector<base::Variant>& rest);

  static void writeln(BuiltIn* module, Optional<std::string> format,
                      const std::vector<base::Variant>& rest);

  static std::string read(BuiltIn* module);
  static std::string readln(BuiltIn* module);

  static base::Variant scoped(
      BuiltIn*, base::Object scope, base::Function function,
      const std::vector<base::Variant>& rest);

  static void exit(BuiltIn*, int status);

  static BuiltIn* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  builtin::Bytes* bytes_;
  builtin::Module* module_;
};

}

#endif // MODULES_BUILTIN_H
