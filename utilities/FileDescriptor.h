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

#ifndef UTILITIES_FILEDESCRIPTOR_H
#define UTILITIES_FILEDESCRIPTOR_H

#include <string>

namespace utilities {

class FileDescriptor {
 public:
  FileDescriptor(int value)
      : value_(value) {
  }

  class EndOfFile {
  };

  class Error {
   public:
    Error(int errnum)
        : errnum(errnum) {
    }
    int errnum;
  };

  static std::string ReadSome(int fd, size_t nbytes, bool async = false);
  /**< Read at most 'nbytes' bytes from 'fd'.  Fewer than 'nbytes' bytes are
       returned only if 'fd' is in non-blocking mode, EOF is reached or 'async'
       is true.  An empty string is only returned if 'fd' is in non-blocking
       mode and there is no data available; an EndOfFile or Error object is
       thrown if nothing can be read because EOF is reached or because of an
       error. */

  static std::string ReadAll(int fd);
  /**< Like ReadSome(), but with no upper limit on the number of bytes read. */

  static bool IsValid(int fd);
  /**< Return true if 'fd' is a valid file descriptor. */

  operator int () const { return value_; }

 private:
  int value_;
};

}

namespace conversions {

template <>
utilities::FileDescriptor as_value(const base::Variant& value,
                                   utilities::FileDescriptor*);

}

#endif // UTILITIES_FILE_H
