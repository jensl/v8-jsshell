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

#ifndef CLASS_H
#define CLASS_H

#include <vector>

#include "v8.h"

namespace glue {
class ConstructorGlue;
}

namespace api {

class Module;

class Class {
 public:
  virtual ~Class();

  template <typename ActualClass>
  class Instance {
   public:
    virtual ~Instance();

    void CreateObject(ActualClass* cls);

    virtual void InitializeObject();

    base::Object GetObject();
    void SetObject(base::Object object);

    ActualClass* GetClass();

    static typename ActualClass::Instance* FromObject(
        ActualClass* cls, base::Object object);
    static typename ActualClass::Instance* FromObjectUnsafe(
        base::Object object);

   protected:
    v8::Persistent<v8::Object> object_;

   private:
    static void CleanUp(v8::Isolate*, v8::Persistent<v8::Value>,
			void* parameter);
  };

  v8::Local<v8::Context> context() { return *context_; }

  bool HasInstance(base::Object object);

  void AddTo(base::Object target);
  void AddTo(Module& module);

  base::Object NewInstance();
  base::Object NewInstance(const std::vector<base::Variant>& argv);

 protected:
  template <typename ActualClass>
  Class(std::string name, ActualClass*);

  template <typename ActualClass, typename ... Arguments>
  Class(std::string name,
        typename ActualClass::Instance* (*callback)(ActualClass*,
                                                    Arguments ...));

  void Inherit(Class* parent);

  template <typename ActualClass, typename Result, typename ... Arguments>
  void AddMethod(std::string name,
                 Result (*callback)(typename ActualClass::Instance*,
                                    Arguments ...));

  template <typename ActualClass, typename Result, typename ... Arguments>
  void AddClassFunction(std::string name,
                        Result (*callback)(ActualClass*, Arguments ...));

  template <typename ActualClass, typename Type>
  void AddProperty(std::string name,
                   Type (*getter)(typename ActualClass::Instance*),
                   void (*setter)(typename ActualClass::Instance*, Type) = NULL);

  template <typename ActualClass, typename Type>
  void AddNamedHandler(
      std::vector<std::string> (*list)(typename ActualClass::Instance*),
      unsigned (*query)(typename ActualClass::Instance*, std::string),
      Type (*getter)(typename ActualClass::Instance*, std::string),
      void (*setter)(typename ActualClass::Instance*, std::string, Type) = NULL);

  template <typename ActualClass, typename Type>
  void AddIndexedHandler(
      unsigned (*query)(typename ActualClass::Instance*, std::uint32_t),
      Type (*getter)(typename ActualClass::Instance*, std::uint32_t),
      void (*setter)(typename ActualClass::Instance*, std::uint32_t, Type) = NULL);

  template <typename ActualClass, typename Type>
  void AddClassProperty(std::string name,
                        Type (*getter)(ActualClass*));

  template <typename ActualClass, typename Type>
  void AddClassProperty(std::string name,
                        Type (*getter)(ActualClass*),
                        void (*setter)(ActualClass*, Type));

  template <typename ActualClass>
  void ConstructObject(typename ActualClass::Instance* instance);

 private:
  Class(std::string name, const glue::ConstructorGlue& glue);

  v8::Persistent<v8::Context> context_;
  v8::Persistent<v8::FunctionTemplate> template_;
  std::string name_;
};

template <typename ActualClass>
Class::Instance<ActualClass>::~Instance() {
}

template <typename ActualClass>
void Class::Instance<ActualClass>::CreateObject(ActualClass* cls) {
  static_cast<Class*>(cls)->NewInstance({ v8::External::New(this) });
}

template <typename ActualClass>
void Class::Instance<ActualClass>::InitializeObject() {
}

template <typename ActualClass>
base::Object Class::Instance<ActualClass>::GetObject() {
  return object_;
}

template <typename ActualClass>
void Class::Instance<ActualClass>::SetObject(base::Object object) {
  object_ = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(),
					    object.handle());
  object_.MakeWeak(v8::Isolate::GetCurrent(), this, &CleanUp);
}

template <typename ActualClass>
ActualClass* Class::Instance<ActualClass>::GetClass() {
  return ActualClass::FromContext(object_->CreationContext());
}

template <typename ActualClass>
void Class::Instance<ActualClass>::CleanUp(
    v8::Isolate*, v8::Persistent<v8::Value>, void* parameter) {
  Class::Instance<ActualClass>* instance =
      static_cast<Class::Instance<ActualClass>*>(parameter);
  instance->object_.Dispose(v8::Isolate::GetCurrent());
  delete instance;
}

template <typename ActualClass>
typename ActualClass::Instance* Class::Instance<ActualClass>::FromObject(
    ActualClass* cls, base::Object object) {
  if (!cls->HasInstance(object))
    throw base::TypeError("wrong this object");
  return static_cast<typename ActualClass::Instance*>(
      object.handle()->GetAlignedPointerFromInternalField(0));
}

template <typename ActualClass>
typename ActualClass::Instance* Class::Instance<ActualClass>::FromObjectUnsafe(
    base::Object object) {
  return static_cast<typename ActualClass::Instance*>(
      object.handle()->GetAlignedPointerFromInternalField(0));
}

}

#include "glue/MethodGlue.h"
#include "glue/FunctionGlue.h"
#include "glue/ConstructorGlue.h"

namespace api {

template <typename ActualClass, typename Result, typename ... Arguments>
void Class::AddMethod(
    std::string name,
    Result (*callback)(typename ActualClass::Instance*, Arguments ...)) {
  glue::MethodGlue(static_cast<ActualClass*>(this), callback)
      .AddTo(name, template_->PrototypeTemplate());
}


template <typename ActualClass, typename Result, typename ... Arguments>
void Class::AddClassFunction(
    std::string name, Result (*callback)(ActualClass*, Arguments ...)) {
  glue::FunctionGlue(static_cast<ActualClass*>(this), callback)
      .AddTo(name, template_->GetFunction());
}

template <typename ActualClass>
Class::Class(std::string name, ActualClass*)
    : Class(name, glue::ConstructorGlue(static_cast<ActualClass*>(this))) {
}

template <typename ActualClass, typename ... Arguments>
Class::Class(
    std::string name,
    typename ActualClass::Instance* (*callback)(ActualClass*, Arguments ...))
    : Class(name, glue::ConstructorGlue(static_cast<ActualClass*>(this),
                                        callback)) {
}

template <typename ActualClass, typename Type>
void Class::AddProperty(std::string name,
                        Type (*getter)(typename ActualClass::Instance*),
                        void (*setter)(typename ActualClass::Instance*, Type)) {
  typedef decltype(getter) Getter;
  typedef decltype(setter) Setter;

  class Wrapper {
   public:
    Wrapper(ActualClass* cls, Getter getter, Setter setter)
        : cls_(cls)
        , getter_(getter)
        , setter_(setter) {
    }

    static v8::Handle<v8::Value> getter(v8::Local<v8::String> property,
                                        const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper*>(
          v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      try {
        return handle_scope.Close(
            conversions::as_result(wrapper->getter_(instance)).handle());
      } catch (base::Error& error) {
        error.Raise();
        return v8::Handle<v8::Value>();
      }
    }

    static void setter(v8::Local<v8::String> property,
                       v8::Local<v8::Value> value_in,
                       const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper*>(
          v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      try {
        wrapper->setter_(instance, base::AsValue<Type>(value_in));
      } catch (base::Error& error) {
        error.Raise();
      }
    }

    void AddTo(std::string name, v8::Handle<v8::FunctionTemplate> target) {
      target->InstanceTemplate()->SetAccessor(
          v8::String::New(name.c_str(), name.length()),
          getter_ ? &getter : NULL,
          setter_ ? &setter : NULL,
          Data(),
          v8::DEFAULT,
          setter_ ? v8::None : v8::ReadOnly,
          v8::AccessorSignature::New(target));
    }

    v8::Handle<v8::Value> Data() { return v8::External::New(this); }

   private:
    ActualClass* cls_;
    Getter getter_;
    Setter setter_;
  };

  (new Wrapper(static_cast<ActualClass*>(this), getter, setter))->AddTo(
      name, template_);
}

template <typename ActualClass, typename Type>
void Class::AddNamedHandler(
    std::vector<std::string> (*list)(typename ActualClass::Instance*),
    unsigned (*query)(typename ActualClass::Instance*, std::string),
    Type (*getter)(typename ActualClass::Instance*, std::string),
    void (*setter)(typename ActualClass::Instance*, std::string, Type)) {
  typedef decltype(list) List;
  typedef decltype(query) Query;
  typedef decltype(getter) Getter;
  typedef decltype(setter) Setter;

  class Wrapper {
   public:
    Wrapper(ActualClass* cls, List list, Query query, Getter getter,
            Setter setter)
        : cls_(cls)
        , list_(list)
        , query_(query)
        , getter_(getter)
        , setter_(setter) {
    }

    static v8::Handle<v8::Array> enumerator(const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      try {
        std::vector<std::string> names(wrapper->list_(instance));
        v8::Handle<v8::Array> array(v8::Array::New(names.size()));
        for (unsigned index = 0; index < names.size(); ++index) {
          const std::string& name(names[index]);
          array->Set(index, v8::String::New(name.c_str(), name.length()));
        }
        return handle_scope.Close(array);
      } catch (base::Error& error) {
        error.Raise();
        return v8::Handle<v8::Array>();
      }
    }

    static v8::Handle<v8::Integer> query(v8::Local<v8::String> property,
                                         const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      v8::String::Utf8Value utf8(property);
      std::string name(*utf8, utf8.length());
      unsigned flags = wrapper->query_(instance, name);
      if ((flags & base::PropertyFlags::kNotFound) != 0)
        return v8::Handle<v8::Integer>();
      int result = v8::None;
      if ((flags & base::PropertyFlags::kWritable) == 0)
        result |= v8::ReadOnly;
      if ((flags & base::PropertyFlags::kEnumerable) == 0)
        result |= v8::DontEnum;
      if ((flags & base::PropertyFlags::kConfigurable) == 0)
        result |= v8::DontDelete;
      return handle_scope.Close(v8::Integer::New(result));
    }

    static v8::Handle<v8::Value> getter(v8::Local<v8::String> property,
                                        const v8::AccessorInfo& info) {
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      if (wrapper->getter_) {
        v8::HandleScope handle_scope;
        typename ActualClass::Instance* instance =
            Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
        v8::String::Utf8Value utf8(property);
        std::string name(*utf8, utf8.length());
        if (wrapper->query_) {
          unsigned flags = wrapper->query_(instance, name);
          if ((flags & base::PropertyFlags::kNotFound) != 0)
            return v8::Handle<v8::Value>();
        }
        return handle_scope.Close(
            conversions::as_result(wrapper->getter_(instance, name)).handle());
      } else {
        return v8::Handle<v8::Value>();
      }
    }

    static v8::Handle<v8::Value> setter(v8::Local<v8::String> property,
                                        v8::Local<v8::Value> value_in,
                                        const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      v8::String::Utf8Value utf8(property);
      std::string name(*utf8, utf8.length());
      try {
        Type value(base::AsValue<Type>(value_in));
        wrapper->setter_(instance, name, value);
        return value_in;
      } catch (base::Error& error) {
        error.Raise();
        return v8::Handle<v8::Value>();
      }
    }

    v8::Handle<v8::Value> Data() { return v8::External::New(this); }

   private:
    ActualClass* cls_;
    List list_;
    Query query_;
    Getter getter_;
    Setter setter_;
  };

  Wrapper* wrapper = new Wrapper(static_cast<ActualClass*>(this), list, query,
                                 getter, setter);

  template_->InstanceTemplate()->SetNamedPropertyHandler(
      &Wrapper::getter,
      setter ? &Wrapper::setter : NULL,
      &Wrapper::query,
      NULL,
      &Wrapper::enumerator,
      wrapper->Data());
}

template <typename ActualClass, typename Type>
void Class::AddIndexedHandler(
    unsigned (*flags)(typename ActualClass::Instance*, std::uint32_t),
    Type (*getter)(typename ActualClass::Instance*, std::uint32_t),
    void (*setter)(typename ActualClass::Instance*, std::uint32_t, Type)) {
  typedef decltype(getter) Getter;
  typedef decltype(setter) Setter;
  typedef decltype(flags) Flags;

  class Wrapper {
   public:
    Wrapper(ActualClass* cls, Getter getter, Setter setter, Flags flags)
        : cls_(cls)
        , getter_(getter)
        , setter_(setter)
        , flags_(flags) {
    }

    static v8::Handle<v8::Value> getter(uint32_t index,
                                        const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          static_cast<typename ActualClass::Instance*>(
              info.This()->GetAlignedPointerFromInternalField(0));
      try {
        return handle_scope.Close(
            conversions::as_result(wrapper->getter_(instance, index)).handle());
      } catch (base::Error& error) {
        error.Raise();
        return v8::Handle<v8::Value>();
      }
    }

    static v8::Handle<v8::Value> setter(uint32_t index,
                                        v8::Local<v8::Value> value_in,
                                        const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      try {
        Type value(base::AsValue<Type>(value_in));
        wrapper->setter_(instance, index, value);
        return value_in;
      } catch (base::Error& error) {
        error.Raise();
        return v8::Handle<v8::Value>();
      }
    }

    static v8::Handle<v8::Integer> query(uint32_t index,
                                         const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          static_cast<typename ActualClass::Instance*>(
              info.This()->GetAlignedPointerFromInternalField(0));
      unsigned flags = wrapper->flags_(instance, index);
      if ((flags & base::PropertyFlags::kNotFound) != 0)
        return v8::Handle<v8::Integer>();
      int result = v8::None;
      if ((flags & base::PropertyFlags::kWritable) == 0)
        result |= v8::ReadOnly;
      if ((flags & base::PropertyFlags::kEnumerable) == 0)
        result |= v8::DontEnum;
      if ((flags & base::PropertyFlags::kConfigurable) == 0)
        result |= v8::DontDelete;
      return handle_scope.Close(v8::Integer::New(result));
    }

    v8::Handle<v8::Value> Data() { return v8::External::New(this); }

   private:
    ActualClass* cls_;
    Getter getter_;
    Setter setter_;
    Flags flags_;
  };

  Wrapper* wrapper = new Wrapper(static_cast<ActualClass*>(this), getter,
                                 setter, flags);

  template_->InstanceTemplate()->SetIndexedPropertyHandler(
      &Wrapper::getter,
      setter ? &Wrapper::setter : NULL,
      &Wrapper::query,
      NULL,
      NULL,
      wrapper->Data());
}

template <typename ActualClass, typename Type>
void Class::AddClassProperty(std::string name,
                             Type (*getter)(ActualClass*)) {
  typedef decltype(getter) Getter;

  class Wrapper {
   public:
    Wrapper(ActualClass* cls, Getter getter)
        : cls_(cls)
        , getter_(getter) {
    }

    static v8::Handle<v8::Value> getter(v8::Local<v8::String> property,
                                        const v8::AccessorInfo& info) {
      v8::HandleScope handle_scope;
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      try {
        return handle_scope.Close(
            conversions::as_result(wrapper->getter_(wrapper->cls_)).handle());
      } catch (base::Error& error) {
        error.Raise();
      } catch (base::NestedException) {
      }
      return v8::Handle<v8::Value>();
    }

    v8::Handle<v8::Value> Data() { return v8::External::New(this); }

   private:
    ActualClass* cls_;
    Getter getter_;
  };

  template_->GetFunction()->SetAccessor(
      v8::String::New(name.c_str(), name.length()),
      &Wrapper::getter,
      NULL,
      (new Wrapper(static_cast<ActualClass*>(this), getter))->Data(),
      v8::DEFAULT,
      v8::ReadOnly);
}

template <typename ActualClass>
void Class::ConstructObject(typename ActualClass::Instance* instance) {
  NewInstance({ v8::External::New(instance) });
}

}

#endif // CLASS_H
