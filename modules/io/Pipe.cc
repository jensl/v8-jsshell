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
#include "modules/io/Pipe.h"

#include <unistd.h>

#include "modules/IO.h"
#include "modules/io/IOError.h"
#include "modules/io/File.h"

namespace modules {
namespace io {

class Pipe::Instance : public api::Class::Instance<Pipe> {
 public:
  Instance() {
    pipefd[0] = -1;
    pipefd[1] = -1;
  }

  virtual void InitializeObject() override;

  int pipefd[2];
};

void Pipe::Instance::InitializeObject() {
  File* file = File::FromContext();

  GetObject().PutHidden("input", file->New(pipefd[0], "r"));
  GetObject().PutHidden("output", file->New(pipefd[1], "a"));
}

Pipe::Pipe()
    : api::Class("Pipe", &constructor) {
  AddProperty<Pipe>("input", &get_input);
  AddProperty<Pipe>("output", &get_output);
}

Pipe* Pipe::FromContext(v8::Handle<v8::Context> context) {
  return IO::FromContext(context)->pipe();
}

Pipe::Instance* Pipe::constructor(Pipe*) {
  Instance* instance = new Instance;

  if (::pipe(instance->pipefd) == -1)
    throw IOError("pipe() failed", errno);

  return instance;
}

File::Instance* Pipe::get_input(Instance* instance) {
  File* file = File::FromContext();
  return file->FromObject(instance->GetObject().GetHidden("input").AsObject());
}

File::Instance* Pipe::get_output(Instance* instance) {
  File* file = File::FromContext();
  return file->FromObject(instance->GetObject().GetHidden("output").AsObject());
}

}
}
