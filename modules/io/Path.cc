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
#include "modules/io/Path.h"

#include "modules/IO.h"
#include "utilities/Path.h"

namespace modules {
namespace io {

class Path::Instance : public Class::Instance<Path> {
 public:
  Instance(std::string value);

  std::string value;
};

Path::Instance::Instance(std::string value)
    : value(value) {
}

Path::Path()
    : Class("Path", constructor) {
  AddProperty<Path>("dirname", get_dirname);
  AddProperty<Path>("basename", get_basename);

  AddClassFunction<Path>("dirname", dirname);
  AddClassFunction<Path>("basename", basename);
  AddClassFunction<Path>("split", split);
  AddClassFunction<Path>("join", join);
  AddClassFunction<Path>("absolute", absolute);

  AddClassProperty<Path>("separator", get_separator);
}

Path::Instance* Path::FromObject(Path* path, base::Object object) {
  return Instance::FromObject(path, object);
}

Path* Path::FromContext(v8::Handle<v8::Context> context) {
  return IO::FromContext(context)->path();
}

Path::Instance* Path::constructor(Path*, std::string value) {
  return new Instance(value);
}

std::string Path::dirname(Path*, std::string path) {
  std::string dirname(utilities::Path::split(path).first);
  if (dirname.length() == 0)
    return ".";
  return dirname;
}

std::string Path::basename(Path*, std::string path) {
  return utilities::Path::split(path).second;
}

std::vector<std::string> Path::split(Path*, std::string path) {
  auto parts(utilities::Path::split(path));
  return { parts.first, parts.second };
}

std::string Path::join(Path*, const std::vector<std::string>& components) {
  return utilities::Path::join(components);
}

std::string Path::absolute(Path*, std::string path) {
  return utilities::Path::absolute(path);
}

std::string Path::get_dirname(Instance* instance) {
  return dirname(NULL, instance->value);
}

std::string Path::get_basename(Instance* instance) {
  return basename(NULL, instance->value);
}

std::string Path::get_separator(Path*) {
  return utilities::Path::separator;
}

}
}
