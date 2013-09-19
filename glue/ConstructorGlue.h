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

#ifndef GLUE_CONSTRUCTOR_H
#define GLUE_CONSTRUCTOR_H

#include "v8.h"

namespace glue {

class ConstructorGlue {
 public:
  template <typename Class>
  ConstructorGlue(Class*);

  template <typename Class, typename ... Arguments>
  ConstructorGlue(Class*,
                  typename Class::Instance* (*impl)(Class*, Arguments ...));

  void SetCallHandler(v8::Handle<v8::FunctionTemplate> target) const;

 private:
  v8::FunctionCallback invocation_callback_;
  v8::Handle<v8::Value> data_;
};

}

#include "glue/CallGlue.h"
#include "glue/WrapperBase.h"

namespace glue {

template <typename ActualClass, typename Callback>
class ConstructorWrapperBase : public WrapperBase<ActualClass, Callback> {
 public:
  ConstructorWrapperBase(ActualClass* cls, Callback callback)
      : WrapperBase<ActualClass, Callback>(cls, callback) {
  }

  virtual base::Variant Invoke(base::Object this_object,
                               const std::vector<base::Variant>& args) override;

  virtual typename ActualClass::Instance* Construct(
      const std::vector<base::Variant>& args) = 0;
};

template <typename ActualClass, typename Callback>
base::Variant ConstructorWrapperBase<ActualClass, Callback>::Invoke(
    base::Object this_object, const std::vector<base::Variant>& args) {
  typename ActualClass::Instance* instance;

  if (args.size() == 1 && args[0].IsExternal()) {
    instance = static_cast<typename ActualClass::Instance*>
        (v8::External::Cast(*args[0].handle())->Value());
  } else {
    instance = Construct(args);
  }

  this_object.handle()->SetAlignedPointerInInternalField(0, instance);
  instance->SetObject(this_object);
  instance->InitializeObject();

  return this_object;
}

template <typename Class>
ConstructorGlue::ConstructorGlue(Class* cls) {
  typedef typename Class::Instance* (*Callback)(Class*);

  class Wrapper : public ConstructorWrapperBase<Class, Callback> {
   public:
    Wrapper(Class* cls)
        : ConstructorWrapperBase<Class, Callback>(cls, &empty) {
    }

    typename Class::Instance* Construct(
        const std::vector<base::Variant>& args) {
      typename Class::Instance* dummy = NULL;
      return Call(args, Wrapper::owner(), Wrapper::callback(), &dummy);
    }

   private:
    static typename Class::Instance* empty(Class*) {
      throw base::TypeError("construction not supported");
    }
  };

  invocation_callback_ = &Wrapper::invocation_callback;
  data_ = (new Wrapper(cls))->Data();
}

template <typename Class, typename ... Arguments>
ConstructorGlue::ConstructorGlue(
    Class* cls, typename Class::Instance* (*impl)(Class*, Arguments ...)) {
  typedef decltype(impl) Callback;

  class Wrapper : public ConstructorWrapperBase<Class, Callback> {
   public:
    Wrapper(Class* cls, Callback callback)
        : ConstructorWrapperBase<Class, Callback>(cls, callback) {
    }

    typename Class::Instance* Construct(
        const std::vector<base::Variant>& args) {
      typename Class::Instance* dummy = NULL;
      return Call(args, Wrapper::owner(), Wrapper::callback(), &dummy);
    }
  };

  invocation_callback_ = &Wrapper::invocation_callback;
  data_ = (new Wrapper(cls, impl))->Data();
}

}

#endif // GLUE_CONSTRUCTOR_H
