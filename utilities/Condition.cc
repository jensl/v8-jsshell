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

#include "utilities/Condition.h"

namespace utilities {

Condition::Lock::Lock(Condition& condition)
    : Mutex::Lock(condition.mutex_)
    , condition_(condition) {
}

void Condition::Lock::Wait() {
  pthread_cond_wait(&condition_.condition_, mutex());
}

void Condition::Lock::Signal() {
  pthread_cond_signal(&condition_.condition_);
}

Condition::Condition(Mutex& mutex)
    : mutex_(mutex) {
  pthread_cond_init(&condition_, NULL);
}

Condition::~Condition() {
  pthread_cond_destroy(&condition_);
}

}
