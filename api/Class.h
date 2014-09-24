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
    v8::Local<v8::Object> object() {
      return v8::Local<v8::Object>::New(CurrentIsolate(), object_);
    }

    v8::Persistent<v8::Object> object_;

   private:
    static void CleanUp(
        const v8::WeakCallbackInfo<Class::Instance<ActualClass>*>&);
  };

  v8::Local<v8::Context> context() {
    return v8::Local<v8::Context>::New(CurrentIsolate(), context_);
  }

  bool HasInstance(base::Object object);

  void AddTo(base::Object target);
  void AddTo(Module& module);

  base::Object NewInstance();
  base::Object NewInstance(const std::vector<base::Variant>& argv);

  base::Object GetConstructor();
  base::Object GetPrototype();

 protected:
  template <typename ActualClass>
  Class(std::string name, ActualClass*);

  template <typename ActualClass, typename ... Arguments>
  Class(std::string name,
        typename ActualClass::Instance* (*callback)(ActualClass*,
                                                    Arguments ...));

  void Inherit(Class* parent);
  void Inherit(base::Object parent);

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

  v8::Local<v8::FunctionTemplate> function_template() {
    return v8::Local<v8::FunctionTemplate>::New(
        CurrentIsolate(), template_);
  }

  v8::Persistent<v8::Context> context_;
  v8::Persistent<v8::FunctionTemplate> template_;
  std::string name_;
};

template <typename ActualClass>
Class::Instance<ActualClass>::~Instance() {
}

template <typename ActualClass>
void Class::Instance<ActualClass>::CreateObject(ActualClass* cls) {
  static_cast<Class*>(cls)->NewInstance(
      { v8::External::New(CurrentIsolate(), this) });
}

template <typename ActualClass>
void Class::Instance<ActualClass>::InitializeObject() {
}

template <typename ActualClass>
base::Object Class::Instance<ActualClass>::GetObject() {
  return object();
}

template <typename ActualClass>
void Class::Instance<ActualClass>::SetObject(base::Object object) {
  object_.Reset(CurrentIsolate(), object.handle());
  object_.SetWeak(this, &CleanUp, v8::WeakCallbackType::kParameter);
}

template <typename ActualClass>
ActualClass* Class::Instance<ActualClass>::GetClass() {
  return ActualClass::FromContext(object()->CreationContext());
}

template <typename ActualClass>
void Class::Instance<ActualClass>::CleanUp(
    const v8::WeakCallbackInfo<Class::Instance<ActualClass>*>& data) {
  data.GetParameter()->object_.Reset();
  delete data.GetParameter();
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
      .AddTo(name, function_template()->PrototypeTemplate());
}


template <typename ActualClass, typename Result, typename ... Arguments>
void Class::AddClassFunction(
    std::string name, Result (*callback)(ActualClass*, Arguments ...)) {
  glue::FunctionGlue(static_cast<ActualClass*>(this), callback)
      .AddTo(name, function_template()->GetFunction());
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

  static void getter(v8::Local<v8::String> property,
                     const v8::PropertyCallbackInfo<v8::Value>& info) {
      v8::EscapableHandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper*>(
          v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      try {
          info.GetReturnValue().Set(handle_scope.Escape(
	      conversions::as_result(wrapper->getter_(instance)).handle()));
      } catch (base::Error& error) {
        error.Raise();
        info.GetReturnValue().Set(v8::Handle<v8::Value>());
      }
    }

    static void setter(v8::Local<v8::String> property,
                       v8::Local<v8::Value> value_in,
                       const v8::PropertyCallbackInfo<void>& info) {
      v8::HandleScope handle_scope(info.GetIsolate());
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
          base::String::New(name.c_str(), name.length()),
          getter_ ? &getter : NULL,
          setter_ ? &setter : NULL,
          Data(),
          v8::DEFAULT,
          setter_ ? v8::None : v8::ReadOnly,
          v8::AccessorSignature::New(CurrentIsolate(), target));
    }

    v8::Handle<v8::Value> Data() {
      return v8::External::New(CurrentIsolate(), this);
    }

   private:
    ActualClass* cls_;
    Getter getter_;
    Setter setter_;
  };

  (new Wrapper(static_cast<ActualClass*>(this), getter, setter))->AddTo(
      name, function_template());
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

   static void enumerator(const v8::PropertyCallbackInfo<v8::Array>& info) {
      v8::EscapableHandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      try {
        std::vector<std::string> names(wrapper->list_(instance));
        v8::Local<v8::Array> array(v8::Array::New(
            info.GetIsolate(), names.size()));
        for (unsigned index = 0; index < names.size(); ++index) {
          const std::string& name(names[index]);
          array->Set(index, base::String::New(name.c_str(), name.length()));
        }
        info.GetReturnValue().Set(handle_scope.Escape(array));
      } catch (base::Error& error) {
        error.Raise();
        info.GetReturnValue().Set(v8::Handle<v8::Array>());
      }
    }

      static void query(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Integer>& info) {
      v8::EscapableHandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      v8::String::Utf8Value utf8(property);
      std::string name(*utf8, utf8.length());
      unsigned flags = wrapper->query_(instance, name);
      if ((flags & base::PropertyFlags::kNotFound) != 0) {
          info.GetReturnValue().Set(v8::Handle<v8::Integer>());
          return;
      }

      int result = v8::None;
      if ((flags & base::PropertyFlags::kWritable) == 0)
        result |= v8::ReadOnly;
      if ((flags & base::PropertyFlags::kEnumerable) == 0)
        result |= v8::DontEnum;
      if ((flags & base::PropertyFlags::kConfigurable) == 0)
        result |= v8::DontDelete;
      info.GetReturnValue().Set(handle_scope.Escape(v8::Integer::New(
          info.GetIsolate(), result)));
    }

    static void getter(v8::Local<v8::String> property,
                       const v8::PropertyCallbackInfo<v8::Value>& info) {
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      if (wrapper->getter_) {
        v8::EscapableHandleScope handle_scope(info.GetIsolate());
        typename ActualClass::Instance* instance =
            Class::Instance<ActualClass>::FromObject(wrapper->cls_,
                                                     info.This());
        v8::String::Utf8Value utf8(property);
        std::string name(*utf8, utf8.length());
        if (wrapper->query_) {
          unsigned flags = wrapper->query_(instance, name);
          if ((flags & base::PropertyFlags::kNotFound) != 0) {
              info.GetReturnValue().Set(v8::Handle<v8::Value>());
              return;
          }
        }
        info.GetReturnValue().Set(handle_scope.Escape(
            conversions::as_result(wrapper->getter_(instance, name)).handle()));
      } else {
        info.GetReturnValue().Set(v8::Handle<v8::Value>());
      }
    }

      static void setter(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value_in,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
      v8::HandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      v8::String::Utf8Value utf8(property);
      std::string name(*utf8, utf8.length());
      try {
        Type value(base::AsValue<Type>(value_in));
        wrapper->setter_(instance, name, value);
        info.GetReturnValue().Set(value_in);
      } catch (base::Error& error) {
        error.Raise();
        info.GetReturnValue().Set(v8::Handle<v8::Value>());
      }
    }

    v8::Handle<v8::Value> Data() {
      return v8::External::New(CurrentIsolate(), this);
    }

   private:
    ActualClass* cls_;
    List list_;
    Query query_;
    Getter getter_;
    Setter setter_;
  };

  Wrapper* wrapper = new Wrapper(static_cast<ActualClass*>(this), list, query,
                                 getter, setter);

  function_template()->InstanceTemplate()->SetNamedPropertyHandler(
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

      static void getter(uint32_t index,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
      v8::EscapableHandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          static_cast<typename ActualClass::Instance*>(
              info.This()->GetAlignedPointerFromInternalField(0));
      try {
        info.GetReturnValue().Set(handle_scope.Escape(
            conversions::as_result(wrapper->getter_(instance, index)).handle()));
      } catch (base::Error& error) {
        error.Raise();
        info.GetReturnValue().Set(v8::Handle<v8::Value>());
      }
    }

      static void setter(uint32_t index,
                         v8::Local<v8::Value> value_in,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
      v8::HandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          Class::Instance<ActualClass>::FromObject(wrapper->cls_, info.This());
      try {
        Type value(base::AsValue<Type>(value_in));
        wrapper->setter_(instance, index, value);
        info.GetReturnValue().Set(value_in);
      } catch (base::Error& error) {
        error.Raise();
        info.GetReturnValue().Set(v8::Handle<v8::Value>());
      }
    }

    static void query(uint32_t index,
                      const v8::PropertyCallbackInfo<v8::Integer>& info) {
      v8::EscapableHandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      typename ActualClass::Instance* instance =
          static_cast<typename ActualClass::Instance*>(
              info.This()->GetAlignedPointerFromInternalField(0));
      unsigned flags = wrapper->flags_(instance, index);
      if ((flags & base::PropertyFlags::kNotFound) != 0) {
        info.GetReturnValue().Set(v8::Handle<v8::Integer>());
        return;
      }

      int result = v8::None;
      if ((flags & base::PropertyFlags::kWritable) == 0)
        result |= v8::ReadOnly;
      if ((flags & base::PropertyFlags::kEnumerable) == 0)
        result |= v8::DontEnum;
      if ((flags & base::PropertyFlags::kConfigurable) == 0)
        result |= v8::DontDelete;
      info.GetReturnValue().Set(handle_scope.Escape(
          v8::Integer::New(info.GetIsolate(), result)));
    }

    v8::Handle<v8::Value> Data() {
      return v8::External::New(CurrentIsolate(), this);
    }

   private:
    ActualClass* cls_;
    Getter getter_;
    Setter setter_;
    Flags flags_;
  };

  Wrapper* wrapper = new Wrapper(static_cast<ActualClass*>(this), getter,
                                 setter, flags);

  function_template()->InstanceTemplate()->SetIndexedPropertyHandler(
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

      static void getter(v8::Local<v8::String> property,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
      v8::EscapableHandleScope handle_scope(info.GetIsolate());
      Wrapper* wrapper = static_cast<Wrapper *>
          (v8::External::Cast(*info.Data())->Value());
      try {
        info.GetReturnValue().Set(handle_scope.Escape(
            conversions::as_result(wrapper->getter_(wrapper->cls_)).handle()));
        return;
      } catch (base::Error& error) {
        error.Raise();
      } catch (base::NestedException) {
      }
      info.GetReturnValue().Set(v8::Handle<v8::Value>());
    }

    v8::Handle<v8::Value> Data() {
      return v8::External::New(CurrentIsolate(), this);
    }

   private:
    ActualClass* cls_;
    Getter getter_;
  };

  function_template()->GetFunction()->SetAccessor(
      base::String::New(name.c_str(), name.length()),
      &Wrapper::getter,
      NULL,
      (new Wrapper(static_cast<ActualClass*>(this), getter))->Data(),
      v8::DEFAULT,
      v8::ReadOnly);
}

template <typename ActualClass>
void Class::ConstructObject(typename ActualClass::Instance* instance) {
  NewInstance({ v8::External::New(CurrentIsolate(), instance) });
}

}

#endif // CLASS_H
