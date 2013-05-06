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
#include "modules/os/Process.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>

#include <map>

#include "modules/Modules.h"
#include "modules/OS.h"
#include "modules/os/OSError.h"
#include "modules/io/IOThread.h"

extern char** environ;

namespace modules {
namespace os {

class Process::Instance : public api::Class::Instance<Process> {
 public:
  Instance()
      : pid(-1)
      , is_running(false)
      , is_self(false)
      , status(0)
      , stdin_source(NULL)
      , stdout_sink(NULL)
      , stderr_sink(NULL) {
  }

  pid_t pid;
  std::string executable;
  std::string cwd;
  std::vector<std::string> argv;
  std::vector<std::string> environ;
  bool is_running;
  bool is_self;
  int status;

  base::Object::Persistent stdin_object;
  base::Object::Persistent stdout_object;
  base::Object::Persistent stderr_object;

  io::IOThread::Source* stdin_source;
  io::IOThread::Sink* stdout_sink;
  io::IOThread::Sink* stderr_sink;
};

Process::Process(const Features& features)
    : api::Class("Process", constructor)
    , shell_disabled_(features.Disabled("OS.Process::shell")) {
  AddMethod<Process>("start", start);
  AddMethod<Process>("wait", wait);
  AddMethod<Process>("kill", kill);
  AddMethod<Process>("run", run);
  AddMethod<Process>("call", call);

  AddProperty<Process>("cwd", get_cwd, set_cwd);
  AddProperty<Process>("stdin", get_stdin, set_stdin);
  AddProperty<Process>("stdout", get_stdout, set_stdout);
  AddProperty<Process>("stderr", get_stderr, set_stderr);
  AddProperty<Process>("pid", get_pid);
  AddProperty<Process>("isRunning", get_isRunning);
  AddProperty<Process>("isSelf", get_isSelf);
  AddProperty<Process>("exitStatus", get_exitStatus);
  AddProperty<Process>("terminationSignal", get_terminationSignal);

  AddClassFunction<Process>("waitpid", waitpid);
  AddClassFunction<Process>("kill", kill);
  AddClassFunction<Process>("fork", fork);
  AddClassFunction<Process>("getenv", getenv);
  AddClassFunction<Process>("sleep", sleep);
  AddClassFunction<Process>("exit", exit);

  AddClassFunction<Process>("getrlimit", getrlimit);
  AddClassFunction<Process>("setrlimit", setrlimit);
}

Process* Process::FromContext(v8::Handle<v8::Context> context) {
  return OS::FromContext(context)->process();
}

namespace {

bool has_option(const std::map<std::string, base::Variant>& options,
                const std::string name) {
  auto iter(options.find(name));
  return iter != options.end();
}

bool get_bool(const std::map<std::string, base::Variant>& options,
              const std::string name, bool default_value = false) {
  auto iter(options.find(name));
  if (iter != options.end())
    return iter->second.AsBoolean();
  else
    return default_value;
}

std::string get_string(const std::map<std::string, base::Variant>& options,
                       const std::string name) {
  return options.at(name).AsString();
}

std::vector<std::string> get_strings(
    const std::map<std::string, base::Variant>& options,
    const std::string name) {
  return base::Array::ToVector<std::string>(options.at(name).AsObject());
}

base::Object get_object(
    const std::map<std::string, base::Variant>& options,
    const std::string name) {
  return options.at(name).AsObject();
}

}

Process::Instance* Process::constructor(
    Process* cls, Optional<std::string> command_opt,
    Optional<std::map<std::string, base::Variant>> options_opt) {
  Instance* instance = new Instance();

  if (command_opt.specified()) {
    std::map<std::string, base::Variant> options;
    if (options_opt.specified())
      options = options_opt.value();

    if (get_bool(options, "shell")) {
      if (cls->shell_disabled_)
        throw base::TypeError("Shell execution disabled");

      instance->executable = "/bin/sh";
      instance->argv = { "/bin/sh", "-c", command_opt.value() };
    } else {
      instance->executable = command_opt.value();

      if (has_option(options, "argv"))
        instance->argv = get_strings(options, "argv");
      else
        instance->argv = { command_opt.value() };
    }

    if (has_option(options, "environ")) {
      base::Object environ(get_object(options, "environ"));

      std::vector<std::string> names(environ.GetOwnPropertyNames());
      for (auto iter(names.begin()); iter != names.end(); ++iter) {
        std::string variable(*iter);
        variable += "=";
        variable += environ.Get(*iter).AsString();
        instance->environ.push_back(variable);
      }
    } else {
      for (char** iter(environ); *iter; ++iter)
        instance->environ.push_back(*iter);
    }

    if (has_option(options, "cwd"))
      instance->cwd = get_string(options, "cwd");
  }

  return instance;
}

std::string Process::get_cwd(Instance* instance) {
  return instance->cwd;
}

void Process::set_cwd(Instance* instance, std::string value) {
  if (instance->pid != -1)
    throw OSError("process already started");
  instance->cwd = value;
}

io::FileObject Process::get_stdin(Instance* instance) {
  return instance->stdin_object.GetObject();
}

void Process::set_stdin(Instance* instance, io::FileObject value) {
  if (instance->pid != -1)
    throw OSError("process already started");
  instance->stdin_object = value.object();
}

io::FileObject Process::get_stdout(Instance* instance) {
  return instance->stdout_object.GetObject();
}

void Process::set_stdout(Instance* instance, io::FileObject value) {
  if (instance->pid != -1)
    throw OSError("process already started");
  instance->stdout_object = value.object();
}

io::FileObject Process::get_stderr(Instance* instance) {
  return instance->stderr_object.GetObject();
}

void Process::set_stderr(Instance* instance, io::FileObject value) {
  if (instance->pid != -1)
    throw OSError("process already started");
  instance->stderr_object = value.object();
}

int Process::get_pid(Instance* instance) {
  return instance->pid;
}

bool Process::get_isRunning(Instance* instance) {
  return instance->is_running;
}

bool Process::get_isSelf(Instance* instance) {
  return instance->is_self;
}

base::Variant Process::get_exitStatus(Instance* instance) {
  if (instance->pid == -1)
    throw OSError("process not started");
  else if (instance->is_running)
    throw OSError("process still running");
  else if (!WIFEXITED(instance->status))
    return base::Variant::Null();

  return base::Variant::Int32(WEXITSTATUS(instance->status));
}

base::Variant Process::get_terminationSignal(Instance* instance) {
  if (instance->pid == -1)
    throw OSError("process not started");
  else if (instance->is_running)
    throw OSError("process still running");
  else if (!WIFSIGNALED(instance->status))
    return base::Variant::Null();

  return base::Variant::Int32(WTERMSIG(instance->status));
}

void Process::start(Instance* instance) {
  if (instance->pid != -1)
    throw OSError("process already started");

  int stdin_fds[2] = { -1, -1 };
  int stdout_fds[2] = { -1, -1 };
  int stderr_fds[2] = { -1, -1 };

  if (!instance->stdin_object.IsEmpty()) {
    io::FileObject file(instance->stdin_object.GetObject());

    if (file.IsMemoryFile()) {
      if (::pipe(stdin_fds) == -1)
        throw OSError("pipe() failed", errno);
    } else if (file.IsFile()) {
      stdin_fds[0] = io::File::Steal(file.GetFile());
    } else if (file.IsSocket()) {
      stdin_fds[0] = io::Socket::fd(file.GetSocket());
    }
  }

  if (!instance->stdout_object.IsEmpty()) {
    io::FileObject file(instance->stdout_object.GetObject());

    if (file.IsMemoryFile()) {
      if (::pipe(stdout_fds) == -1)
        throw OSError("pipe() failed", errno);
    } else if (file.IsFile()) {
      stdout_fds[1] = io::File::Steal(file.GetFile());
    } else if (file.IsSocket()) {
      stdout_fds[1] = io::Socket::fd(file.GetSocket());
    }
  }

  if (!instance->stderr_object.IsEmpty()) {
    io::FileObject file(instance->stderr_object.GetObject());

    if (file.IsMemoryFile()) {
      if (::pipe(stderr_fds) == -1)
        throw OSError("pipe() failed", errno);
    } else if (file.IsFile()) {
      stderr_fds[1] = io::File::Steal(file.GetFile());
    } else if (file.IsSocket()) {
      stderr_fds[1] = io::Socket::fd(file.GetSocket());
    }
  }

  pid_t pid = ::fork();

  if (pid == -1)
    throw OSError("fork() failed", errno);

  if (pid == 0) {
    if (stdin_fds[0] != -1) {
      ::dup2(stdin_fds[0], 0);
      ::close(stdin_fds[0]);
    }
    if (stdin_fds[1] != -1)
      ::close(stdin_fds[1]);

    if (stdout_fds[1] != -1) {
      ::dup2(stdout_fds[1], 1);
      ::close(stdout_fds[1]);
    }
    if (stdout_fds[0] != -1)
      ::close(stdout_fds[0]);

    if (stderr_fds[1] != -1) {
      ::dup2(stderr_fds[1], 2);
      ::close(stderr_fds[1]);
    }
    if (stderr_fds[0] != -1)
      ::close(stderr_fds[0]);

    if (instance->cwd.length() != 0)
      ::chdir(instance->cwd.c_str());

    if (!instance->executable.empty()) {
      std::vector<char*> argv;
      for (auto iter = instance->argv.begin();
           iter != instance->argv.end();
           ++iter)
        argv.push_back(const_cast<char*>(iter->c_str()));
      argv.push_back(NULL);

      std::vector<char*> environ;
      for (auto iter = instance->environ.begin();
           iter != instance->environ.end();
           ++iter)
        environ.push_back(const_cast<char*>(iter->c_str()));
      environ.push_back(NULL);

      execve(instance->executable.c_str(), argv.data(), environ.data());

      int errnum = errno;

      perror("execve() failed");

      ::exit(errnum);
    }

    instance->pid = getpid();
    instance->is_running = true;
    instance->is_self = true;

    instance->stdin_object = base::Object();
    instance->stdout_object = base::Object();
    instance->stderr_object = base::Object();
  } else {
    if (stdin_fds[0] != -1)
      ::close(stdin_fds[0]);
    if (stdout_fds[1] != -1)
      ::close(stdout_fds[1]);
    if (stderr_fds[1] != -1)
      ::close(stderr_fds[1]);

    if (!instance->stdin_object.IsEmpty()) {
      io::FileObject file(instance->stdin_object.GetObject());

      if (file.IsMemoryFile()) {
        instance->stdin_source = io::IOThread::AddSource(
            stdin_fds[1], io::MemoryFile::value(file.GetMemoryFile()));
      }
    }

    if (!instance->stdout_object.IsEmpty()) {
      io::FileObject file(instance->stdout_object.GetObject());

      if (file.IsMemoryFile())
        instance->stdout_sink = io::IOThread::AddSink(stdout_fds[0]);
    }

    if (!instance->stderr_object.IsEmpty()) {
      io::FileObject file(instance->stderr_object.GetObject());

      if (file.IsMemoryFile())
        instance->stderr_sink = io::IOThread::AddSink(stderr_fds[0]);
    }

    instance->pid = pid;
    instance->is_running = true;
  }
}

bool Process::wait(Instance* instance, Optional<bool> nohang) {
  if (instance->pid == -1)
    throw OSError("process not started");
  else if (instance->is_self)
    throw OSError("waiting on self would deadlock");

  int options = nohang.value(false) ? WNOHANG : 0;
  pid_t pid = ::waitpid(instance->pid, &instance->status, options);

  if (pid == -1)
    throw OSError("waitpid() failed", errno);
  else if (pid == 0)
    return false;

  instance->is_running = false;

  if (instance->stdout_sink) {
    instance->stdout_sink->Wait();

    io::FileObject file(instance->stdout_object.GetObject());
    io::MemoryFile::set_value(file.GetMemoryFile(),
                              instance->stdout_sink->GetData());

    delete instance->stdout_sink;
    instance->stdout_sink = NULL;
  }

  if (instance->stderr_sink) {
    instance->stderr_sink->Wait();

    io::FileObject file(instance->stderr_object.GetObject());
    io::MemoryFile::set_value(file.GetMemoryFile(),
                              instance->stderr_sink->GetData());

    delete instance->stderr_sink;
    instance->stderr_sink = NULL;
  }

  return true;
}

void Process::kill(Instance* instance, int signal) {
  if (instance->pid == -1)
    throw OSError("process not started");

  if (::kill(instance->pid, signal) == -1)
    throw OSError("kill() failed", errno);
}

void Process::run(Instance* instance) {
  start(instance);
  wait(instance, false);

  if (WIFEXITED(instance->status)) {
    int exitstatus = WEXITSTATUS(instance->status);
    if (exitstatus != 0)
      throw OSError("process exited with status ") << exitstatus;
  } else {
    int signal = WTERMSIG(instance->status);
    throw OSError("process terminated by signal ") << signal;
  }
}

std::string Process::call(Instance* instance, Optional<std::string> stdin_opt) {
  io::MemoryFile* memory_file = io::MemoryFile::FromContext(
      instance->GetClass()->context());

  if (stdin_opt.specified()) {
    if (!instance->stdin_object.IsEmpty())
      throw OSError("process has stdin set already");
    instance->stdin_object = memory_file->Create(stdin_opt.value(), "r");
  }

  if (!instance->stdout_object.IsEmpty())
    throw OSError("process has stdout set");

  base::Object stdout(memory_file->Create("", "w"));
  instance->stdout_object = stdout;

  run(instance);

  return io::MemoryFile::value(memory_file->FromObject(stdout));
}

base::Object Process::waitpid(Process*, int pid, int options) {
  int status;
  pid_t finished = ::waitpid(pid, &status, options);

  if (finished == -1)
    throw OSError("waitpid() failed", errno);
  else if (finished == 0)
    return base::Object();

  base::Object result(base::Object::Create());

  result.Put("pid", base::Variant::Int32(finished));

  if (WIFEXITED(status))
    result.Put("exitStatus", WEXITSTATUS(status));
  else
    result.Put("terminationSignal", WTERMSIG(status));

  return result;
}

void Process::kill(Process*, int pid, int signal) {
  if (::kill(pid, signal) == -1)
    throw OSError("kill() failed", errno);
}

int Process::fork(Process*) {
  pid_t pid = ::fork();

  if (pid == -1)
    throw OSError("fork() failed", errno);

  return pid;
}

std::string Process::getenv(Process*, std::string name) {
  char* value = ::getenv(name.c_str());
  if (value)
    return value;
  else
    throw base::ReferenceError("Environment variable not defined: ") << name;
}

namespace {

void ToTimespec(struct timespec& ts, double time) {
  ts.tv_sec = static_cast<long>(time / 1000);
  ts.tv_nsec = static_cast<long>((time - ts.tv_sec * 1000) * 1000000);
}

double FromTimespec(const struct timespec& ts) {
  return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

}

double Process::sleep(Process*, double time) {
  struct timespec req, rem;

  ToTimespec(req, time);

  if (::nanosleep(&req, &rem) == -1) {
    if (errno == EINTR)
      return FromTimespec(rem);
    else
      throw OSError("nanosleep() failed", errno);
  }

  return 0;
}

void Process::exit(Process*, int returncode) {
  ::exit(returncode);
}

namespace {

int ResourceFromName(std::string name) {
  if (name == "address-space" || name == "RLIMIT_AS")
    return RLIMIT_AS;
  else if (name == "core" || name == "RLIMIT_CORE")
    return RLIMIT_CORE;
  else if (name == "cpu" || name == "RLIMIT_CPU")
    return RLIMIT_CPU;
  else if (name == "data" || name == "RLIMIT_DATA")
    return RLIMIT_DATA;
  else if (name == "file-size" || name == "RLIMIT_FSIZE")
    return RLIMIT_FSIZE;
  else if (name == "open-files" || name == "RLIMIT_NOFILE")
    return RLIMIT_NOFILE;
  else if (name == "rss" || name == "RLIMIT_RSS")
    return RLIMIT_RSS;
  else if (name == "stack" || name == "RLIMIT_STACK")
    return RLIMIT_STACK;
  else
    throw base::SyntaxError("invalid resource name: ") << name;
}

}

base::Object Process::getrlimit(Process*, std::string name) {
  struct rlimit values;

  if (::getrlimit(ResourceFromName(name), &values) == -1)
    throw OSError("getrlimit() failed", errno);

  base::Object result(base::Object::Create());

  result.Put("current", static_cast<std::int64_t>(values.rlim_cur));
  result.Put("maximum", static_cast<std::int64_t>(values.rlim_max));

  return result;
}

void Process::setrlimit(Process*, std::string name, std::int64_t soft,
			Optional<std::int64_t> hard) {
  int resource = ResourceFromName(name);
  struct rlimit values;

  if (::getrlimit(resource, &values) == -1)
    throw OSError("getrlimit() failed", errno);

  values.rlim_cur = soft;

  if (hard.specified())
    values.rlim_max = hard.value();

  if (::setrlimit(resource, &values) == -1)
    throw OSError("setrlimit() failed", errno);
}

}
}
