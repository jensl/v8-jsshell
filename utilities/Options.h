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

#ifndef UTILITIES_OPTIONS_H
#define UTILITIES_OPTIONS_H

#include <map>
#include <string>

namespace utilities {

class Options {
 public:
  Options();
  Options(base::Object object);

  bool Has(std::string name) const;

  bool GetBoolean(std::string name, bool default_value = false) const;
  int GetInt32(std::string name, int default_value = 0) const;
  unsigned GetUInt32(std::string name, unsigned default_value = 0) const;
  double GetNumber(std::string name, double default_value = 0) const;
  std::string GetString(std::string name, std::string default_value = "") const;
  base::Object GetObject(std::string name) const;

  typedef std::map<std::string, base::Variant>::const_iterator const_iterator;

  const_iterator begin() const;
  const_iterator end() const;

 private:
  std::map<std::string, base::Variant> values_;
};

}

namespace conversions {

template <>
utilities::Options as_value(const base::Variant& value, utilities::Options*);

template <>
utilities::Options as_argument(const std::vector<base::Variant>& args,
                               unsigned index, utilities::Options*);

}

#endif // UTILITIES_OPTIONS_H
