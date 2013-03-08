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

#ifndef UTILITIES_FILE_H
#define UTILITIES_FILE_H

#include <sys/types.h>
#include <sys/stat.h>

#include <string>

namespace utilities {

class File {
 public:
  class Error {
   public:
    Error(std::string prefix, int errnum = 0);

    const std::string& message() { return message_; }

   private:
    std::string message_;
  };

  File(std::string filename, std::string mode);
  ~File();

  std::string read(std::string encoding = "");
  void write(std::string data);
  void close();

  time_t mtime();

  FILE* operator-> () { return file_; }
  operator FILE* () { return file_; }

 private:
  std::string filename_;
  FILE* file_;
};

}

#endif // UTILITIES_FILE_H
