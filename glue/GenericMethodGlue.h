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

#ifndef GLUE_GENERICMETHODGLUE_H
#define GLUE_GENERICMETHODGLUE_H

#include <string>
#include <vector>

#include "v8.h"

namespace glue {

class GenericMethodGlue {
 public:
  template <typename Class, typename Result, typename ... Arguments>
  GenericMethodGlue(Class* cls, Result (*impl)(typename Class::Value,
                                               Arguments ...));

  void AddTo(std::string name, v8::Handle<v8::Template> target);

 private:
  v8::FunctionCallback invocation_callback_;
  v8::Handle<v8::Value> data_;
};

}

#include "glue/CallGlue.h"
#include "glue/WrapperBase.h"

namespace glue {

template <typename Class, typename Result, typename ... Arguments>
GenericMethodGlue::GenericMethodGlue(
    Class* cls, Result (*impl)(typename Class::Value, Arguments ...)) {
  typedef decltype(impl) Callback;

  class Wrapper : public WrapperBase<Class, Callback> {
   public:
    Wrapper(Class* cls, Callback callback)
        : WrapperBase<Class, Callback>(cls, callback) {
    }

    virtual base::Variant Invoke(base::Object this_object,
                                 const std::vector<base::Variant>& args)
        override {
      base::Variant* dummy = NULL;
      return Call(args, base::AsValue<typename Class::Value>(this_object),
                  Wrapper::callback(), dummy);
    }
  };

  invocation_callback_ = &Wrapper::invocation_callback;
  data_ = (new Wrapper(cls, impl))->Data();
}

}

#endif // GLUE_METHODGLUE_H
