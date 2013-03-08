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

#include "Base.h"
#include "api/GC.h"

namespace api {

namespace {

static bool is_running = false;

void BeforeGC(v8::GCType, v8::GCCallbackFlags) {
  is_running = true;
}

void AfterGC(v8::GCType, v8::GCCallbackFlags) {
  is_running = false;
}

}

void GC::Initialize() {
  v8::V8::AddGCPrologueCallback(BeforeGC);
  v8::V8::AddGCEpilogueCallback(AfterGC);
}

bool GC::IsRunning() {
  return is_running;
}

}
