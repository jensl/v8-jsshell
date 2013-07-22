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
#include "modules/builtin/Module.h"

#include <cassert>
#include <memory>

#include "api/Runtime.h"
#include "modules/Modules.h"
#include "modules/BuiltIn.h"
#include "modules/IO.h"
#include "modules/OS.h"
#include "modules/PostgreSQL.h"
#include "modules/URL.h"
#include "modules/ZLib.h"
#include "modules/Testing.h"
#include "utilities/File.h"
#include "utilities/Path.h"
#include "utilities/CompileFile.h"

namespace modules {
namespace builtin {

class GlobalObject : public api::Class {
 public:
  class Instance : public api::Class::Instance<GlobalObject> {
   public:
    Instance(Module::Instance* module);

    Module::Instance* module;
  };

  GlobalObject();

  base::Object Create(Module::Instance* module);

 private:
  static std::vector<std::string> list(Instance* instance);
  static unsigned query(Instance* instance, std::string name);
  static base::Variant get(Instance* instance, std::string name);
  static void set(Instance* instance, std::string name, base::Variant value);
};

class Module::Instance : public api::Class::Instance<Module> {
 public:
  Instance(const Features& features);

  void InitializeObject() override;

  api::Runtime runtime;
  bool stopped;

  BuiltIn* builtin;
  IO* io;
  OS* os;
#if POSTGRESQL_SUPPORT
  PostgreSQL* postgresql;
#endif
#if LIBCURL_SUPPORT
  URL* url;
#endif
#if ZLIB_SUPPORT
  ZLib* zlib;
#endif
  Testing* testing;

  std::map<std::string, bool> features;

  std::string path;
};

GlobalObject::Instance::Instance(Module::Instance* module)
    : module(module) {
}

GlobalObject::GlobalObject()
    : api::Class("GlobalObject", this) {
  AddNamedHandler<GlobalObject>(list, query, get, set);
}

base::Object GlobalObject::Create(Module::Instance* module) {
  Instance* instance = new Instance(module);
  instance->CreateObject(this);
  return instance->GetObject();
}

std::vector<std::string> GlobalObject::list(Instance* instance) {
  api::Runtime::Select select(instance->module->runtime);
  return instance->module->runtime.GetGlobalObject().GetPropertyNames();
}

unsigned GlobalObject::query(Instance* instance, std::string name) {
  api::Runtime::Select select(instance->module->runtime);
  return instance->module->runtime.GetGlobalObject().GetPropertyFlags(name);
}

base::Variant GlobalObject::get(Instance* instance, std::string name) {
  api::Runtime::Select select(instance->module->runtime);
  return instance->module->runtime.GetGlobalObject().Get(name);
}

void GlobalObject::set(Instance* instance, std::string name,
                       base::Variant value) {
  api::Runtime::Select select(instance->module->runtime);
  return instance->module->runtime.GetGlobalObject().Put(name, value);
}

Module::Instance::Instance(const Features& features)
    : stopped(false)
    , builtin(NULL)
    , io(NULL)
    , os(NULL)
#if POSTGRESQL_SUPPORT
    , postgresql(NULL)
#endif
#if LIBCURL_SUPPORT
    , url(NULL)
#endif
#if ZLIB_SUPPORT
    , zlib(NULL)
#endif
    , testing(NULL)
    , features(std::map<std::string, bool>(features.begin(), features.end())) {
  runtime.Start();

  api::Runtime::Select select(runtime);

  builtin = new BuiltIn;
  builtin->AddToRuntime(runtime);

  if (!features.Disabled("IO")) {
    io = new IO(features);
    io->AddToRuntime(runtime);
  }

  if (!features.Disabled("OS")) {
    os = new OS(features);
    os->AddToRuntime(runtime);
  }

#if POSTGRESQL_SUPPORT
  if (!features.Disabled("PostgreSQL")) {
    postgresql = new PostgreSQL(features);
    postgresql->AddToRuntime(runtime);
  }
#endif

#if LIBCURL_SUPPORT
  if (!features.Disabled("URL")) {
    url = new URL(features);
    url->AddToRuntime(runtime);
  }
#endif

#if ZLIB_SUPPORT
  if (!features.Disabled("ZLib")) {
    zlib = new ZLib(features);
    zlib->AddToRuntime(runtime);
  }
#endif

  if (features.Enabled("Testing")) {
    testing = new Testing(features);
    testing->AddToRuntime(runtime);
  }
}

void Module::Instance::InitializeObject() {
  assert(GetClass()->HasInstance(GetObject()));
  runtime.context()->SetEmbedderData(kModuleObject, GetObject().handle());
}

Module::Module()
    : Class("Module", &constructor) {
  AddMethod<Module>("load", &load);
  AddMethod<Module>("eval", &eval);
  AddMethod<Module>("close", &close);
  AddProperty<Module>("global", &get_global);
  AddProperty<Module>("path", &get_path);

  AddClassFunction<Module>("assign", &assign);
  AddClassFunction<Module>("load", &load);
  AddClassProperty<Module>("global", &get_global);
  AddClassProperty<Module>("path", &get_path);

  global_object_ = new GlobalObject;
}

Module::Instance* Module::GetInstance() {
  base::Object object(base::Variant(
      v8::Context::GetCurrent()->GetEmbedderData(kModuleObject)).AsObject());

  return Instance::FromObjectUnsafe(object);
}

api::Runtime& Module::runtime(Instance* instance) {
  return instance->runtime;
}

Module::Instance* Module::Root(const Features& features) {
  Instance* instance = new Instance(features);
  api::Runtime::Select select(instance->runtime);
  instance->CreateObject(instance->builtin->module());
  return instance;
}

base::Variant Module::Load(Instance* instance, std::string path,
                           std::string encoding) {
  api::Runtime::Select select(instance->runtime);
  v8::Handle<v8::Script> script;

  path = utilities::Path::join({ instance->path, path });

  try {
    script = utilities::CompileFile(path, encoding);
  } catch (utilities::File::Error error) {
    throw base::TypeError(error.message());
  }

  if (script.IsEmpty())
    return base::Variant();

  std::string previous_load_path(instance->path);
  instance->path = utilities::Path::split(path).first;
  base::Variant result(script->Run());
  instance->path = previous_load_path;

  return result;
}

base::Variant Module::Eval(Instance* instance, std::string source) {
  api::Runtime::Select select(instance->runtime);
  v8::Handle<v8::Script> script(
      v8::Script::Compile(v8::String::New(source.c_str(), source.length())));

  if (script.IsEmpty())
    throw base::NestedException();

  return script->Run();
}

Module* Module::FromContext(v8::Handle<v8::Context> context) {
  return BuiltIn::FromContext(context)->module();
}

Module::Instance* Module::constructor(
    Module* module, Optional<std::map<std::string, bool>> features_opt) {
  std::map<std::string, bool> requested_features;
  if (features_opt.specified())
    requested_features = features_opt.value();

  const Features& current_features(module->GetInstance()->features);
  for (auto iter(current_features.begin());
       iter != current_features.end();
       ++iter) {
    if (!iter->second)
      requested_features[iter->first] = false;
  }

  return new Instance(requested_features);
}

base::Variant Module::load(Instance* instance, std::string path,
                           Optional<std::string> encoding_opt) {
  if (instance->stopped)
    throw base::TypeError("module has been closed");

  std::string encoding;

  if (encoding_opt.specified())
    encoding = encoding_opt.value();

  return Load(instance, path, encoding);
}

base::Variant Module::eval(Instance* instance, std::string source) {
  if (instance->stopped)
    throw base::TypeError("module has been closed");

  return Eval(instance, source);
}

base::Variant Module::load(Module* cls, std::string path,
                           Optional<std::string> encoding_opt) {
  std::string encoding;

  if (encoding_opt.specified())
    encoding = encoding_opt.value();

  return Load(cls->GetInstance(), path, encoding);
}

void Module::close(Instance* instance) {
  instance->runtime.Stop();
  instance->stopped = true;
}

void Module::assign(Module* cls, std::string name, base::Variant value) {
  cls->GetInstance()->GetObject().Put(name, value);
}

base::Object Module::get_global(Instance* instance) {
  if (instance->stopped)
    throw base::TypeError("module has been closed");

  return instance->GetClass()->global_object_->Create(instance);
}

std::string Module::get_path(Instance* instance) {
  return instance->path;
}

base::Object Module::get_global(Module* cls) {
  return cls->GetInstance()->runtime.GetGlobalObject();
}

std::string Module::get_path(Module* cls) {
  return cls->GetInstance()->path;
}

}
}
