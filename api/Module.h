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

#ifndef API_MODULE_H
#define API_MODULE_H

#include <string>
#include <vector>

#include "v8.h"

namespace api {

class Runtime;

class Module {
 public:
  virtual ~Module();

  void AddToRuntime(Runtime& runtime);

  virtual void ExtendObject(base::Object target) = 0;
  virtual void ExtendRuntime(Runtime& runtime);

  virtual base::Object GetObject();

 protected:
  Module(int index, std::string name);

  void AddTo(base::Object target);

  template <typename ActualModule, typename Result, typename ... Arguments>
  void AddFunction(base::Object target, const char* name,
                   Result (*callback)(ActualModule*, Arguments ...));

  template <typename ActualModule>
  static ActualModule* FromContext(
      int index, v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  int index_;
  std::string name_;

  v8::Local<v8::FunctionTemplate> function_template() {
    return v8::Local<v8::FunctionTemplate>::New(
      CurrentIsolate(), template_);
  }

  v8::Persistent<v8::FunctionTemplate> template_;
};

template <typename ActualModule>
ActualModule* Module::FromContext(int index, v8::Handle<v8::Context> context) {
  if (context.IsEmpty())
    context = CurrentIsolate()->GetCurrentContext();
  return static_cast<ActualModule*>(static_cast<Module*>(
      v8::External::Cast(*context->GetEmbedderData(index))->Value()));
}

}

#include "glue/FunctionGlue.h"

namespace api {

template <typename ActualModule, typename Result, typename ... Arguments>
void Module::AddFunction(
    base::Object target, const char* name,
    Result (*callback)(ActualModule*, Arguments ...)) {
  glue::FunctionGlue function_glue(static_cast<ActualModule*>(this), callback);
  function_glue.AddTo(name, target);
}

}

#endif // MODULE_H
