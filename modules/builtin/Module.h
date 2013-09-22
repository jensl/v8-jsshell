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

#ifndef MODULES_BUILTIN_MODULE_H
#define MODULES_BUILTIN_MODULE_H

#include <map>
#include <string>

#include "api/Class.h"

namespace api {
class Runtime;
}

namespace modules {

class Features;

namespace builtin {

class GlobalObject;

class Module : public api::Class {
 public:
  class Instance;

  Module();
  virtual ~Module();

  Instance* GetInstance();

  static void Release(Instance* instance);

  static api::Runtime& runtime(Instance* instance);

  static Instance* Root(const Features& features);

  static base::Variant Load(Instance* instance, std::string path,
                            std::string encoding = "utf-8");
  static base::Variant Eval(Instance* instance, std::string source);

  static Module* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(
      Module*, Optional<std::map<std::string, bool>> flags);

  static base::Variant load(Instance* instance, std::string path,
                            Optional<std::string> encoding);
  static base::Variant eval(Instance* instance, std::string source);
  static void close(Instance* instance);

  static void assign(Module* cls, std::string name, base::Variant value);
  static base::Variant load(Module* cls, std::string path,
                            Optional<std::string> encoding);

  static base::Object get_global(Instance* instance);
  static std::string get_path(Instance* instance);
  static base::Object get_global(Module* cls);
  static std::string get_path(Module* cls);

  GlobalObject* global_object_;
};

}
}

#endif // MODULES_BUILTIN_BYTES_H
