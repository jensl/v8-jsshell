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

#ifndef UTILITIES_FORMATTER_H
#define UTILITIES_FORMATTER_H

#include <string>
#include <vector>

namespace utilities {

class Formatter {
 public:
  std::string Format(std::string format,
                     const std::vector<base::Variant>& values);

 protected:
  virtual std::string Replace(std::string directive,
                              const base::Variant& value);

  virtual base::Variant GetNextValue();
  virtual base::Variant GetProperty(std::string name);

 private:
  std::vector<base::Variant>::const_iterator values_iter_;
  std::vector<base::Variant>::const_iterator values_end_;
  base::Object current_object_;
};

}

#endif // UTILITIES_FORMATTER_H
