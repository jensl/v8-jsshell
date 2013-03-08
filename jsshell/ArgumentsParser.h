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

#ifndef JSSHELL_ARGUMENTSPARSER_H
#define JSSHELL_ARGUMENTSPARSER_H

#include <string>
#include <vector>
#include <set>
#include <map>

class ArgumentsParser;

class Argument {
 public:
  Argument(ArgumentsParser& parser, std::string argument);

  bool is_switch() const { return is_switch_; }
  bool is_option() const { return is_option_; }
  bool is_argument() const { return !is_switch_ && !is_option_; }

  const std::string& name() const { return name_; }
  const std::string& value() const { return value_; }

 private:
  std::string name_;
  std::string value_;
  bool is_switch_;
  bool is_option_;
};

class ArgumentsParser {
 public:
  void AddAlias(char short_name, std::string long_name);
  void Parse(const std::vector<std::string>& argv);

  bool GetSwitch(std::string name) const;
  bool HasOption(std::string name) const;

  const std::vector<Argument>& GetOption(std::string name) const;

  typedef std::vector<Argument>::const_iterator const_iterator;

  const_iterator begin();
  const_iterator end();

 private:
  friend class Argument;

  std::string ExpandName(std::string name);

  std::map<char, std::string> aliases_;
  std::set<std::string> switches_;
  std::map<std::string, std::vector<Argument>> options_;
  std::vector<Argument> arguments_;
};

#endif
