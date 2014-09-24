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

#include "utilities/File.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <memory>

namespace utilities {

File::Error::Error(std::string prefix, int errnum)
    : message_(prefix) {
  if (errnum != 0) {
    message_ += ": ";
    message_ += strerror(errnum);
  }
}

File::File(std::string filename, std::string mode)
    : filename_(filename)
    , file_(::fopen(filename.c_str(), mode.c_str())) {
  if (!file_)
    throw Error(filename, errno);
}

File::~File() {
  if (file_)
    fclose(file_);
}

std::string File::read(std::string encoding) {
  std::string result;
  char buffer[4096];

  while (!feof(file_) && !ferror(file_)) {
    size_t nread = fread(buffer, 1, sizeof buffer, file_);
    if (nread > 0)
      result.append(buffer, nread);
  }

  if (ferror(file_))
    throw Error(filename_, errno);

  return result;
}

void File::write(std::string data) {
  size_t written = 0;
  while (written < data.length() && !ferror(file_)) {
    written += fwrite(data.c_str() + written, 1,
                      data.length() - written, file_);
  }
  if (ferror(file_))
    throw Error(filename_, errno);
}

void File::close() {
  fclose(file_);
  file_ = NULL;
}

time_t File::mtime() {
  struct stat buf;

  if (fstat(fileno(file_), &buf) == -1)
    throw Error(filename_, errno);

  return buf.st_mtime;
}

}
