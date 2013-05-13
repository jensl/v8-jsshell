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
#include "utilities/Formatter.h"

#include <cctype>
#include <cstdio>
#include <sstream>

namespace utilities {

std::string Formatter::Format(
    std::string format, const std::vector<base::Variant>& values) {
  values_iter_ = values.begin();
  values_end_ = values.end();

  std::string::iterator ch(format.begin());
  std::ostringstream result;

  base::Variant value;

  while (ch != format.end()) {
    if (*ch != '%') {
      result << *ch++;
      continue;
    }

    ++ch;

    if (ch == format.end()) {
      throw base::SyntaxError("incomplete formatting directive at end of format");
    } else if (*ch == '%') {
      result << *ch++;
      continue;
    } else if (*ch == '(') {
      ++ch;

      std::string name;

      while (true) {
        if (ch == format.end())
          throw base::SyntaxError("incomplete formatting directive at end of format");
        else if (*ch == ')')
          break;

        name += *ch++;
      }

      ++ch;

      if (name.length() == 0)
        throw base::SyntaxError("invalid named formatting directive, empty name");

      value = GetProperty(name);
    } else {
      value = GetNextValue();
    }

    std::string directive("%");

    do {
      if (ch == format.end())
        throw base::SyntaxError("incomplete formatting directive at end of format");

      directive += *ch;
    } while (!std::isalpha(*ch++));

    result << Replace(directive, value);
  }

  return result.str();
}

std::string Formatter::Replace(std::string directive,
                               const base::Variant& value) {
  if (directive == "%s")
    return value.AsString();

  char buffer[1024];
  char* bufferp = buffer;
  int written, length = sizeof buffer;

  do {
    switch (*directive.rbegin()) {
      case 'd':
      case 'i':
        written = snprintf(bufferp, length, directive.c_str(),
                           value.AsInt32());
        break;

      case 'o':
      case 'x': case 'X':
        written = snprintf(bufferp, length, directive.c_str(),
                           value.AsUInt32());
        break;

      case 'e': case 'E':
      case 'f': case 'F':
      case 'g': case 'G':
        written = snprintf(bufferp, length, directive.c_str(),
                           value.AsNumber());
        break;

      case 's':
        written = snprintf(bufferp, length, directive.c_str(),
                           value.AsString().c_str());
        break;

      case 'r':
        written = snprintf(
            bufferp, length,
            (directive.substr(0, directive.length() - 1) + "s").c_str(),
            value.AsJSON().c_str());
        break;

      default:
        throw base::SyntaxError("unsupported formatting directive");
    }

    if (written < length)
      break;

    if (bufferp != buffer)
      delete[] bufferp;

    length = written + 1;
    bufferp = new char[length];
  } while (true);

  std::string result(bufferp, written);

  if (bufferp != buffer)
    delete[] bufferp;

  return result;
}

base::Variant Formatter::GetNextValue() {
  if (values_iter_ == values_end_)
    throw base::TypeError("too few parameters for format");

  current_object_ = base::Object();
  return *values_iter_++;
}

base::Variant Formatter::GetProperty(std::string name) {
  if (current_object_.IsEmpty()) {
    base::Variant value = GetNextValue();
    if (!value.IsObject())
      throw base::TypeError("invalid parameters for format, expected object");
    current_object_ = value.AsObject();
  }

  if (!current_object_.HasProperty(name))
    throw base::TypeError("property named by formatting directive missing from parameter object");

  return current_object_.Get(name);
}

}
