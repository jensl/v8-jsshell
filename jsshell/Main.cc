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

#include <string>
#include <vector>
#include <map>

extern int Main(const std::vector<std::string>& argv,
                const std::map<std::string, std::string>& environ);

int main(int argc, char** argv_in, char** environ_in) {
  std::vector<std::string> argv;
  std::map<std::string, std::string> environ;

  for (int argi = 0; argi < argc; ++argi)
    argv.push_back(argv_in[argi]);

  while (*environ_in) {
    std::string variable(*environ_in);
    std::string::size_type eq(variable.find("="));
    std::string name(variable, 0, eq);
    std::string value(variable, eq + 1, std::string::npos);

    environ[name] = value;

    ++environ_in;
  }

  return Main(argv, environ);
}
