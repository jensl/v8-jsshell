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

#ifndef BASE_H
#define BASE_H

extern void stop();

#include "v8.h"

inline v8::Isolate* CurrentIsolate() {
  return v8::Isolate::GetCurrent();
}

inline v8::Local<v8::Context> CurrentContext() {
  return CurrentIsolate()->GetCurrentContext();
}

template<typename T>
inline void Check(v8::Maybe<T> value) {
  value.ToChecked();
}

template<typename T>
inline void Check(v8::MaybeLocal<T> value) {
  value.ToLocalChecked();
}

#include "base/PropertyFlags.h"
#include "base/Variant.h"
#include "base/Object.h"
#include "base/Array.h"
#include "base/Function.h"
#include "base/Error.h"
#include "base/Convert.h"
#include "base/String.h"

#include "base/VariantImpl.h"
#include "base/ObjectImpl.h"
#include "base/ArrayImpl.h"

#endif // BASE_H
