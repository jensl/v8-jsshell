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

#ifndef UTILITIES_MUTEX_H
#define UTILITIES_MUTEX_H

#include <pthread.h>

namespace utilities {

class Mutex {
 public:
  class Error {};

  class Lock {
   public:
    Lock(Mutex& mutex);
    ~Lock();

    void Release();
    void Acquire();

   protected:
    pthread_mutex_t* mutex() { return &mutex_.mutex_; }

    Mutex& mutex_;
    bool locked_;
  };

  Mutex();
  ~Mutex();

 private:
  friend class Lock;
  pthread_mutex_t mutex_;
};

}

#endif // UTILITIES_MUTEX_H
