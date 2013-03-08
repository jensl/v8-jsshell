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

#include "utilities/Mutex.h"

namespace utilities {

Mutex::Lock::Lock(Mutex& mutex)
    : mutex_(mutex)
    , locked_(true) {
  pthread_mutex_lock(&mutex_.mutex_);
}

Mutex::Lock::~Lock() {
  if (locked_)
    pthread_mutex_unlock(&mutex_.mutex_);
}

void Mutex::Lock::Release() {
  pthread_mutex_unlock(&mutex_.mutex_);
  locked_ = false;
}

void Mutex::Lock::Acquire() {
  pthread_mutex_lock(&mutex_.mutex_);
  locked_ = true;
}

Mutex::Mutex()
    : mutex_() {
  pthread_mutex_init(&mutex_, NULL);
}

Mutex::~Mutex() {
  pthread_mutex_destroy(&mutex_);
}

}
