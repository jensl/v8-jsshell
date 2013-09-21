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

#ifndef GLUE_CALLGLUE_H
#define GLUE_CALLGLUE_H

#include <vector>

#include "v8.h"

namespace glue {

template <typename Result, typename ReturnType>
ReturnType Return(Result result, ReturnType*) {
  return result;
}

template <typename Result>
base::Variant Return(Result result, base::Variant*) {
  return base::AsResult(result);
}

template <typename ReturnType>
ReturnType ReturnVoid(ReturnType*) {
  return ReturnType();
}

inline base::Variant ReturnVoid(base::Variant*) {
  return base::Variant::Undefined();
}

template <typename An>
std::vector<An> Rest(const std::vector<base::Variant>& argv, unsigned offset) {
  std::vector<An> rest;
  for (unsigned index = offset; index < argv.size(); ++index)
    rest.push_back(base::AsValue<An>(argv[index]));
  return rest;
}

template <typename ReturnType, typename Result, typename Context>
ReturnType Call(const std::vector<base::Variant>& args, Context context,
                Result (*callback)(Context), ReturnType* dummy,
                unsigned offset = 0) {
  return Return(callback(context), dummy);
}

template <typename ReturnType, typename Context>
ReturnType Call(const std::vector<base::Variant>& args, Context context,
                void (*callback)(Context), ReturnType* dummy,
                unsigned offset = 0) {
  callback(context);
  return ReturnVoid(dummy);
}

template <typename ReturnType, typename Result, typename Context, typename An>
ReturnType Call(const std::vector<base::Variant>& args, Context context,
                Result (*callback)(Context, const std::vector<An>&),
                ReturnType* dummy, unsigned offset = 0) {
  return Return(callback(context, Rest<An>(args, offset)), dummy);
}

template <typename ReturnType, typename Context, typename An>
ReturnType Call(const std::vector<base::Variant>& args, Context context,
                void (*callback)(Context, const std::vector<An>&),
                ReturnType* dummy, unsigned offset = 0) {
  callback(context, Rest<An>(args, offset));
  return ReturnVoid(dummy);
}

template <typename ReturnType, typename Result, typename Context, typename A0,
          typename ... Arguments>
ReturnType Call(const std::vector<base::Variant>& args, Context context,
                Result (*callback)(Context, A0, Arguments ...),
                ReturnType* dummy, unsigned offset = 0) {
  typedef decltype(callback) Callback;

  class Partial {
   public:
    Partial(Callback callback, Context context,
            const std::vector<base::Variant>& args, int index)
        : callback_(callback)
        , context_(context)
        , argument_(base::AsArgument<A0>(args, index)) {
    }
    static Result Apply(Partial* partial, Arguments ... arguments) {
      return partial->callback_(
          partial->context_, partial->argument_, arguments ...);
    }
   private:
    Callback callback_;
    Context context_;
    A0 argument_;
  };

  Partial partial(callback, context, args, offset);
  return Call(args, &partial, &Partial::Apply, dummy, offset + 1);
}

template <typename Context>
base::Variant Call(const std::vector<base::Variant>& args, Context context,
                   void (*callback)(Context), base::Variant* dummy,
                   unsigned offset = 0) {
  callback(context);
  return base::Variant::Undefined();
}

template <typename Context, typename An>
base::Variant Call(const std::vector<base::Variant>& args, Context context,
                   void (*callback)(Context, const std::vector<An>&),
                   base::Variant* dummy, unsigned offset = 0) {
  callback(context, Rest<An>(args, offset));
  return base::Variant::Undefined();
}

}

#endif // GLUE_CALLGLUE_H
