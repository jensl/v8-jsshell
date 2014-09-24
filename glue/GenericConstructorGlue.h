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

#ifndef GLUE_GENERICCONSTRUCTORGLUE_H
#define GLUE_GENERICCONSTRUCTORGLUE_H

#include "v8.h"

namespace glue {

class GenericConstructorGlue {
 public:
  template <typename Class, typename ... Arguments>
  GenericConstructorGlue(Class*,
                         typename Class::Value (*impl)(Class*, Arguments ...));

  void SetCallHandler(v8::Handle<v8::FunctionTemplate> target) const;

 private:
  v8::FunctionCallback invocation_callback_;
  v8::Handle<v8::Value> data_;
};

}

#include "glue/CallGlue.h"
#include "glue/WrapperBase.h"

namespace glue {

template <typename Class, typename ... Arguments>
GenericConstructorGlue::GenericConstructorGlue(
    Class* cls, typename Class::Value (*impl)(Class*, Arguments ...)) {
  typedef decltype(impl) Callback;

  class Wrapper : public WrapperBase<Class, Callback> {
   public:
    Wrapper(Class* cls, Callback callback)
        : WrapperBase<Class, Callback>(cls, callback) {
    }

    base::Variant Invoke(
        base::Object this_object, const std::vector<base::Variant>& args) {
      base::Variant* dummy = NULL;
      return Call(args, Wrapper::owner(), Wrapper::callback(), dummy);
    }
  };

  invocation_callback_ = &Wrapper::invocation_callback;
  data_ = (new Wrapper(cls, impl))->Data();
}

}

#endif // GLUE_CONSTRUCTOR_H
