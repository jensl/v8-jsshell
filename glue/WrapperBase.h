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

#ifndef GLUE_WRAPPERBASE_H
#define GLUE_WRAPPERBASE_H

namespace glue {

template <typename Owner, typename Callback>
class WrapperBase {
 public:
  typedef Callback CallbackType;

  WrapperBase(Owner* owner, Callback callback)
      : owner_(owner)
      , callback_(callback) {
  }

  Owner* owner() { return owner_; }
  Callback callback() { return callback_; }

  static void invocation_callback(
      const v8::FunctionCallbackInfo<v8::Value> &info) {
    v8::EscapableHandleScope handle_scope(info.GetIsolate());
    WrapperBase<Owner, Callback>* wrapper =
        static_cast<WrapperBase<Owner, Callback>*>(
            v8::External::Cast(*info.Data())->Value());
    try {
      std::vector<base::Variant> argv;
      for (int index = 0; index < info.Length(); ++index)
        argv.push_back(info[index]);
      info.GetReturnValue().Set(
          handle_scope.Escape(wrapper->Invoke(info.This(), argv).handle()));
    } catch (base::Error& error) {
      error.Raise();
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    } catch (base::NestedException) {
      info.GetReturnValue().Set(v8::Undefined(info.GetIsolate()));
    }
  }

  v8::Handle<v8::Value> Data();

  virtual base::Variant Invoke(base::Object this_object,
                               const std::vector<base::Variant>& argv) = 0;

 private:
  Owner* owner_;
  Callback callback_;
};

template <typename Owner, typename Callback>
v8::Handle<v8::Value> WrapperBase<Owner, Callback>::Data() {
  return v8::External::New(CurrentIsolate(), this);
}

}

#endif // GLUE_WRAPPERBASE_H
