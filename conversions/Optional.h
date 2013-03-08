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

#ifndef CONVERSIONS_OPTIONAL_H
#define CONVERSIONS_OPTIONAL_H

template <typename T>
class Optional {
 public:
  Optional()
      : specified_(false) {}
  Optional(const Optional<T>& other)
      : value_(other.value_)
      , specified_(other.specified_) {}
  Optional(T value)
      : value_(value)
      , specified_(true) {}

  bool specified() const { return specified_; }
  T value() { return value_; }
  T value(const T& def) { return specified_ ? value_ : def; }
  const T value() const { return value_; }

 private:
  T value_;
  bool specified_;
};

namespace conversions {

template <typename R>
Optional<R> as_argument(const std::vector<base::Variant>& args, unsigned index,
                        Optional<R>*) {
  if (args.size() <= index)
    return Optional<R>();
  return Optional<R>(as_value(args[index], static_cast<R*>(NULL)));
}

}

#endif // CONVERSIONS_OPTIONAL_H
