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

#include "utilities/Thread.h"

namespace {

void* StartRoutine(void* thread) {
  static_cast<utilities::Thread*>(thread)->Run();
  return NULL;
}

}

namespace utilities {

Thread::Thread()
    : thread_(0) {
}

void Thread::Start() {
  int error = pthread_create(&thread_, NULL, &StartRoutine, this);

  if (error != 0)
    throw Error();
}

void Thread::Join() {
  int error = pthread_join(thread_, NULL);

  if (error != 0)
    throw Error();
}

}
