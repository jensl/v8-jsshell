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

#ifndef MODULES_POSTGRESQL_FORMATTER_H
#define MODULES_POSTGRESQL_FORMATTER_H

#include <list>
#include <string>
#include <vector>

#include "utilities/Formatter.h"

namespace modules {
namespace postgresql {

class Formatter : public utilities::Formatter {
 public:
  Formatter();

  const std::vector<const char*>& parameters() const { return parameters_; }
  const std::vector<int>& lengths() const { return lengths_; }
  const std::vector<int>& formats() const { return formats_; }

 protected:
  virtual std::string Replace(std::string directive,
                              const base::Variant& value) override;

  virtual unsigned ParameterIndex();

 private:
  void Add(std::string value, bool binary);
  void AddNull();

  std::list<std::string> values_;
  std::vector<const char*> parameters_;
  std::vector<int> lengths_;
  std::vector<int> formats_;
};

class PrepareFormatter : public Formatter {
 public:
  PrepareFormatter();

 protected:
  virtual base::Variant GetNextValue() override;
  virtual base::Variant GetProperty(std::string name) override;

  virtual unsigned ParameterIndex() override;

 private:
  unsigned parameters_;
};

}
}

#endif // MODULES_POSTGRESQL_FORMATTER_H
