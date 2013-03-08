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

#ifndef API_RUNTIME_H
#define API_RUNTIME_H

#include "v8.h"

namespace api {

class Runtime {
 public:
  class Select {
   public:
    explicit Select(Runtime& runtime);

   private:
    v8::HandleScope handle_scope_;
    v8::Context::Scope context_scope_;
  };

  virtual ~Runtime();

  void Start();
  void Stop();

  base::Object GetGlobalObject();

  v8::Handle<v8::Context> context() { return context_; }

 private:
  v8::Persistent<v8::Context> context_;
};

}

#endif // API_RUNTIME_H
