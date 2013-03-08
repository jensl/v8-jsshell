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

#ifndef MODULES_IO_PATH_H
#define MODULES_IO_PATH_H

#include "api/Class.h"

namespace modules {
namespace io {

class Path : public api::Class {
 public:
  class Instance;

  Path();

  static Instance* FromObject(Path* module, base::Object object);

  static Path* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(Path*, std::string path);

  static std::string toString(Instance* instance);

  static std::string dirname(Path*, std::string path);
  static std::string basename(Path*, std::string path);
  static std::vector<std::string> split(Path*, std::string path);
  static std::string join(Path*, const std::vector<std::string>& components);
  static std::string absolute(Path*, std::string path);

  static std::string get_dirname(Instance* instance);
  static std::string get_basename(Instance* instance);

  static std::string get_separator(Path*);
};

}
}

namespace conversions {

using namespace modules::io;

template <>
Path::Instance* as_value(const base::Variant& value, Path::Instance**);

template <>
base::Variant as_result(Path::Instance* result);

}

#endif // MODULES_IO_PATH_H
