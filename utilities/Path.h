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

#ifndef UTILITIES_PATH_H
#define UTILITIES_PATH_H

#include <string>
#include <vector>

namespace utilities {

class Path {
 public:
  class Error {
   public:
    Error(std::string prefix, int errnum = 0);

    const std::string& message() { return message_; }

   private:
    std::string message_;
  };

  static const char* const separator;

  static std::pair<std::string, std::string> split(std::string path);
  static std::string join(std::vector<std::string> components);
  static std::string absolute(std::string path);
  static std::string getcwd();
};

}

#endif // UTILITIES_PATH_H
