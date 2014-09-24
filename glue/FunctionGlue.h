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

#ifndef GLUE_FUNCTIONGLUE_H
#define GLUE_FUNCTIONGLUE_H

#include <string>
#include <vector>

namespace glue {

class FunctionGlue {
 public:
  template <typename Owner, typename Result, typename ... Arguments>
  FunctionGlue(Owner* module, Result (*impl)(Owner*, Arguments ...));

  void AddTo(std::string name, base::Object target);

 private:
  v8::FunctionCallback invocation_callback_;
  v8::Handle<v8::Value> data_;
};

}

#include "glue/CallGlue.h"
#include "glue/WrapperBase.h"

namespace glue {

template <typename Owner, typename Result, typename ... Arguments>
FunctionGlue::FunctionGlue(Owner* module, Result (*impl)(Owner*,
                                                         Arguments ...)) {
  typedef decltype(impl) Callback;

  class Wrapper : public WrapperBase<Owner, Callback> {
   public:
    Wrapper(Owner* module, Callback callback)
        : WrapperBase<Owner, Callback>(module, callback) {
    }

    virtual base::Variant Invoke(base::Object this_object,
                                 const std::vector<base::Variant>& argv)
        override {
      base::Variant* dummy = NULL;
      return Call(argv, Wrapper::owner(), Wrapper::callback(), dummy);
    }
  };

  invocation_callback_ = &Wrapper::invocation_callback;
  data_ = (new Wrapper(module, impl))->Data();
}

}

#endif // GLUE_FUNCTIONGLUE_H
