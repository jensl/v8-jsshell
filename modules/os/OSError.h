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

#ifndef MODULES_OS_OSERROR_H
#define MODULES_OS_OSERROR_H

class OSError : public base::CustomError {
 public:
  OSError(std::string message);
  OSError(std::string prefix, int errnum);

  OSError& operator<< (const std::string& string);
  OSError& operator<< (int integer);
};

#endif // MODULES_IO_IOERROR_H
