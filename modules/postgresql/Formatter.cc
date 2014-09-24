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
#include "modules/postgresql/Formatter.h"

#include <stdio.h>

#include "modules/builtin/Bytes.h"

namespace modules {
namespace postgresql {

Formatter::Formatter() {
}

std::string Formatter::Replace(std::string directive,
                               const base::Variant& value) {
  char conversion = *directive.rbegin();

  if (!value.IsEmpty()) {
    if (value.IsUndefined() || value.IsNull()) {
      AddNull();
    } else if (conversion == 't') {
      if (!value.IsDateObject())
        throw base::TypeError("invalid argument; expected Date object");
      double seconds = value.AsNumber() / 1000;
      Add(base::Variant::Number(seconds).AsString() + " seconds", false);
    } else if (directive == "%b") {
      builtin::Bytes::Value bytes =
          base::AsValue<builtin::Bytes::Value>(value.handle());
      Add(bytes, true);
    } else {
      Add(utilities::Formatter::Replace(directive, value), false);
    }
  }

  char buffer[12];
  snprintf(buffer, sizeof buffer, "$%u", static_cast<unsigned>(ParameterIndex()));
  if (conversion == 't')
    return std::string("('epoch'::timestamp + ") + buffer + "::interval)";
  return std::string(buffer);
}

unsigned Formatter::ParameterIndex() {
  return parameters_.size();
}

void Formatter::Add(std::string value, bool binary) {
  values_.push_back(value);
  parameters_.push_back(values_.back().c_str());
  lengths_.push_back(value.length());
  formats_.push_back(binary);
}

void Formatter::AddNull() {
  parameters_.push_back(NULL);
  lengths_.push_back(0);
  formats_.push_back(0);
}

PrepareFormatter::PrepareFormatter()
    : parameters_(0) {
}

base::Variant PrepareFormatter::GetNextValue() {
  return base::Variant();
}

base::Variant PrepareFormatter::GetProperty(std::string name) {
  return base::Variant();
}

unsigned PrepareFormatter::ParameterIndex() {
  return ++parameters_;
}

}
}
