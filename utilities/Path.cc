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

#include "utilities/Path.h"

#include <unistd.h>
#include <string.h>

namespace utilities {

Path::Error::Error(std::string prefix, int errnum)
    : message_(prefix) {
  if (errnum != 0) {
    message_ += ": ";
    message_ += strerror(errnum);
  }
}

const char* const Path::separator = "/";

std::pair<std::string, std::string> Path::split(std::string path) {
  if (path.empty())
    return std::make_pair("", "");

  std::string::size_type pos = path.length();

  while (pos > 0 && path[pos - 1] == separator[0])
    --pos;

  if (!pos)
    return std::make_pair(separator, separator);

  std::string::size_type delim = path.rfind(separator[0], pos - 1);
  if (delim == std::string::npos)
    return std::make_pair("", path.substr(0, pos));
  else if (delim == 0)
    return std::make_pair(separator, path.substr(1, pos));

  return std::make_pair(
      path.substr(0, delim), path.substr(delim + 1, pos - delim - 1));
}

std::string Path::join(std::vector<std::string> components) {
  if (components.size() == 0)
    return "";

  auto iter(components.begin());

  std::string result(*iter++);

  while (iter != components.end()) {
    if (result.length() != 0 && result[result.length() - 1] != separator[0])
      result += separator[0];

    std::string component = *iter;

    if (component.length() != 0 && component[0] == separator[0])
      result = component;
    else
      result += component;

    ++iter;
  }

  return result;
}

std::string Path::absolute(std::string path) {
  path = join({ getcwd(), path });

  auto parts(split(path));

  if (parts.first == "/")
    return parts.first + parts.second;
  else if (parts.second == ".")
    return absolute(parts.first);
  else if (parts.second == "..")
    return split(absolute(parts.first)).first;
  else
    return join({ absolute(parts.first), parts.second });
}

std::string Path::getcwd() {
  size_t bufsize = 256;
  char* buf = NULL;

  while (true) {
    buf = static_cast<char*>(malloc(bufsize));

    if (::getcwd(buf, bufsize))
      break;

    int errnum = errno;

    free(buf);

    if (errnum != ERANGE)
      throw Error("getcwd() failed", errnum);

    bufsize *= 2;
  }

  std::string result(buf);
  free(buf);
  return result;
}

}
