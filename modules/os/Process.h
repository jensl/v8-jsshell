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

#ifndef MODULES_OS_PROCESS_H
#define MODULES_OS_PROCESS_H

#include <string>
#include <vector>

#include "api/Module.h"
#include "modules/io/FileObject.h"

namespace modules {

class Features;

namespace os {

class Process : public api::Class {
 public:
  class Instance;

  Process(const Features &features);

  static Process* FromContext(
      v8::Handle<v8::Context> context = v8::Handle<v8::Context>());

 private:
  static Instance* constructor(
      Process* cls, Optional<std::string> command_opt,
      Optional<std::map<std::string, base::Variant>> options_opt);

  static std::string get_cwd(Instance* instance);
  static void set_cwd(Instance* instance, std::string value);

  static io::FileObject get_stdin(Instance* instance);
  static void set_stdin(Instance* instance, io::FileObject value);

  static io::FileObject get_stdout(Instance* instance);
  static void set_stdout(Instance* instance, io::FileObject value);

  static io::FileObject get_stderr(Instance* instance);
  static void set_stderr(Instance* instance, io::FileObject value);

  static int get_pid(Instance* instance);
  static bool get_isRunning(Instance* instance);
  static bool get_isSelf(Instance* instance);
  static base::Variant get_exitStatus(Instance* instance);
  static base::Variant get_terminationSignal(Instance* instance);

  static void start(Instance* instance);
  static bool wait(Instance* instance, Optional<bool> nohang);
  static void kill(Instance* instance, int signal);
  static void run(Instance* instance);
  static std::string call(Instance* instance, Optional<std::string> stdin);

  static base::Object waitpid(Process*, int pid, int flags);
  static void kill(Process*, int pid, int signal);
  static int fork(Process*);
  static std::string getenv(Process*, std::string name);
  static double sleep(Process*, double time);

  static base::Object getrlimit(Process*, std::string name);
  static void setrlimit(Process*, std::string name, std::int64_t soft,
			Optional<std::int64_t> hard);

  bool shell_disabled_;
};

}
}

#endif // MODULES_OS_PROCESS_H
