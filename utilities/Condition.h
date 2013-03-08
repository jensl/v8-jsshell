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

#ifndef UTILITIES_CONDITION_H
#define UTILITIES_CONDITION_H

#include <pthread.h>

#include "utilities/Mutex.h"

namespace utilities {

class Condition {
 public:
  class Error {};

  class Lock : public Mutex::Lock {
   public:
    Lock(Condition& condition);

    void Wait();
    void Signal();

   private:
    Lock(const Lock&);
    Condition& condition_;
  };

  Condition(Mutex& mutex);
  ~Condition();

 private:
  friend class Lock;
  Mutex& mutex_;
  pthread_cond_t condition_;
};

}

#endif // UTILITIES_CONDITION_H
