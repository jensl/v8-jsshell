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

#include "jsshell/ArgumentsParser.h"

Argument::Argument(ArgumentsParser& parser, std::string argument)
    : is_switch_(false)
    , is_option_(false) {
  if (argument[0] == '-') {
    if (argument[1] == '-') {
      std::string::size_type equal(argument.find("=", 2));
      name_ = parser.ExpandName(argument.substr(2, equal - 2));
      if (equal != std::string::npos) {
        is_option_ = true;
        value_ = argument.substr(equal + 1);
      } else {
        is_switch_ = true;
      }
    } else {
      name_ = parser.ExpandName(argument.substr(1, 1));
      if (argument.length() > 2) {
        is_option_ = true;
        value_ = argument.substr(2);
      } else {
        is_switch_ = true;
      }
    }
  } else {
    value_ = argument;
  }
}

void ArgumentsParser::AddAlias(char short_name, std::string long_name) {
  aliases_.insert(std::make_pair(short_name, long_name));
}

void ArgumentsParser::Parse(const std::vector<std::string>& argv) {
  for (auto iter(++argv.begin()); iter != argv.end(); ++iter) {
    arguments_.push_back(Argument(*this, *iter));
    Argument& argument(arguments_.back());
    if (argument.is_switch())
      switches_.insert(argument.name());
    else if (argument.is_option())
      options_[argument.name()].push_back(argument);
  }
}

bool ArgumentsParser::GetSwitch(std::string name) const {
  auto iter(switches_.find(name));
  if (iter == switches_.end())
    return false;
  return true;
}

bool ArgumentsParser::HasOption(std::string name) const {
  return options_.find(name) != options_.end();
}

const std::vector<Argument>& ArgumentsParser::GetOption(
    std::string name) const {
  return options_.at(name);
}

ArgumentsParser::const_iterator ArgumentsParser::begin() {
  return arguments_.begin();
}

ArgumentsParser::const_iterator ArgumentsParser::end() {
  return arguments_.end();
}

std::string ArgumentsParser::ExpandName(std::string name) {
  if (name.length() == 1) {
    auto iter(aliases_.find(name[0]));
    if (iter != aliases_.end())
      return iter->second;
  }
  return name;
}
