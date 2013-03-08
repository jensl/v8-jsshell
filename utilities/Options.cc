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
#include "utilities/Options.h"

namespace utilities {

Options::Options() {
}

Options::Options(base::Object object) {
  std::vector<std::string> names(object.GetOwnPropertyNames());
  for (auto iter(names.begin()); iter != names.end(); ++iter)
    values_[*iter] = object.Get(*iter);
}

bool Options::Has(std::string name) const {
  return values_.find(name) != values_.end();
}

bool Options::GetBoolean(std::string name, bool default_value) const {
  auto iter(values_.find(name));
  if (iter != values_.end())
    return iter->second.AsBoolean();
  else
    return default_value;
}

int Options::GetInt32(std::string name, int default_value) const {
  auto iter(values_.find(name));
  if (iter != values_.end())
    return iter->second.AsInt32();
  else
    return default_value;
}

unsigned Options::GetUInt32(std::string name, unsigned default_value) const {
  auto iter(values_.find(name));
  if (iter != values_.end())
    return iter->second.AsUInt32();
  else
    return default_value;
}

double Options::GetNumber(std::string name, double default_value) const {
  auto iter(values_.find(name));
  if (iter != values_.end())
    return iter->second.AsNumber();
  else
    return default_value;
}

std::string Options::GetString(std::string name,
                               std::string default_value) const {
  auto iter(values_.find(name));
  if (iter != values_.end())
    return iter->second.AsString();
  else
    return default_value;
}

base::Object Options::GetObject(std::string name) const {
  auto iter(values_.find(name));
  if (iter != values_.end())
    return iter->second.AsObject();
  else
    return base::Object();
}

Options::const_iterator Options::begin() const {
  return values_.begin();
}

Options::const_iterator Options::end() const {
  return values_.end();
}

}

namespace conversions {

template <>
utilities::Options as_value(const base::Variant& value, utilities::Options*) {
  if (!value.IsNull() && !value.IsUndefined())
    return utilities::Options(value.AsObject());
  else
    return utilities::Options();
}

template <>
utilities::Options as_argument(const std::vector<base::Variant>& args,
                               unsigned index, utilities::Options* dummy) {
  if (args.size() <= index)
    return utilities::Options();
  else
    return as_value(args[index], dummy);
}

}
