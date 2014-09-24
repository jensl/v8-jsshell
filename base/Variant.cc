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
#include "conversions/Optional.h"

namespace base {

Variant::Variant() {
}

bool Variant::IsEmpty() const {
  return handle_.IsEmpty();
}

bool Variant::IsUndefined() const {
  return handle_->IsUndefined();
}

bool Variant::IsNull() const {
  return handle_->IsNull();
}

bool Variant::IsBoolean() const {
  return handle_->IsBoolean();
}

bool Variant::IsNumber() const {
  return handle_->IsNumber();
}

bool Variant::IsInteger() const {
  return IsInt32() || IsUInt32();
}

bool Variant::IsInt32() const {
  return handle_->IsInt32();
}

bool Variant::IsUInt32() const {
  return handle_->IsUint32();
}

bool Variant::IsString() const {
  return handle_->IsString();
}

bool Variant::IsObject() const {
  return handle_->IsObject();
}

bool Variant::IsArrayObject() const {
  return handle_->IsArray();
}

bool Variant::IsFunctionObject() const {
  return handle_->IsFunction();
}

bool Variant::IsBooleanObject() const {
  return handle_->IsBooleanObject();
}

bool Variant::IsNumberObject() const {
  return handle_->IsNumberObject();
}

bool Variant::IsStringObject() const {
  return handle_->IsStringObject();
}

bool Variant::IsDateObject() const {
  return handle_->IsDate();
}

bool Variant::IsRegExpObject() const {
  return handle_->IsRegExp();
}

bool Variant::IsExternal() const {
  return handle_->IsExternal();
}

bool Variant::IsArrayBuffer() const {
  return handle_->IsArrayBuffer();
}

bool Variant::IsArrayBufferView() const {
  return handle_->IsArrayBufferView();
}

bool Variant::IsTypedArray() const {
  return handle_->IsTypedArray();
}

bool Variant::IsUint8Array() const {
  return handle_->IsUint8Array();
}

bool Variant::IsUint8ClampedArray() const {
  return handle_->IsUint8ClampedArray();
}

bool Variant::IsInt8Array() const {
  return handle_->IsInt8Array();
}

bool Variant::IsUint16Array() const {
  return handle_->IsUint16Array();
}

bool Variant::IsInt16Array() const {
  return handle_->IsInt16Array();
}

bool Variant::IsUint32Array() const {
  return handle_->IsUint32Array();
}

bool Variant::IsInt32Array() const {
  return handle_->IsInt32Array();
}

bool Variant::IsFloat32Array() const {
  return handle_->IsFloat32Array();
}

bool Variant::IsFloat64Array() const {
  return handle_->IsFloat64Array();
}

namespace {

enum class Hint {
  String,
  Number,
  None
};

Variant ConvertToPrimitive(Variant value, Hint hint) {
  if (!value.IsObject())
    return value;

  const char* const string_first[2] = { "toString", "valueOf" };
  const char* const number_first[2] = { "valueOf", "toString" };
  const char* const* order;

  if (hint == Hint::None) {
    if (value.IsDateObject())
      hint = Hint::String;
    else
      hint = Hint::Number;
  }

  if (hint == Hint::String)
    order = string_first;
  else
    order = number_first;

  base::Object object(value.AsObject());
  Variant converted(value);

  if (object.HasProperty(order[0]))
    converted = object.Call(order[0]);

  if (converted.IsObject() && object.HasProperty(order[1]))
    converted = object.Call(order[1]);

  if (converted.IsObject())
    throw TypeError("Cannot convert object to primitive value");

  return converted;
}

}

bool Variant::AsBoolean() const {
  return handle_->BooleanValue();
}

double Variant::AsNumber() const {
  if (IsObject())
    return ConvertToPrimitive(*this, Hint::Number).AsNumber();

  return handle_->NumberValue();
}

std::int64_t Variant::AsInteger() const {
  if (IsObject())
    return ConvertToPrimitive(*this, Hint::Number).AsInteger();

  return handle_->IntegerValue();
}

std::int32_t Variant::AsInt32() const {
  if (IsObject())
    return ConvertToPrimitive(*this, Hint::Number).AsInt32();

  return handle_->Int32Value();
}

std::uint32_t Variant::AsUInt32() const {
  if (IsObject())
    return ConvertToPrimitive(*this, Hint::Number).AsUInt32();

  return handle_->Uint32Value();
}

std::string Variant::AsString() const {
  if (IsObject())
    return ConvertToPrimitive(*this, Hint::String).AsString();

  v8::String::Utf8Value utf8(handle_);
  return std::string(*utf8, utf8.length());
}

base::Object Variant::AsObject() const {
  v8::Local<v8::Object> handle(handle_->ToObject());
  if (handle.IsEmpty())
    throw NestedException();
  return base::Object(handle);
}

std::string Variant::AsJSON() const {
  base::Object json(base::Object::GlobalObject().Get("JSON").AsObject());

  return json.Call("stringify", { *this }).AsString();
}

namespace {

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
 public:
  virtual void* Allocate(size_t length) override {
    char* buffer = new char[length];
    std::fill(buffer, buffer + length, 0);
    return buffer;
  }

  virtual void* AllocateUninitialized(size_t length) override {
    return new char[length];
  }

  virtual void Free(void* data, size_t length) override {
    delete[] static_cast<char*>(data);
  }
};

ArrayBufferAllocator* array_buffer_allocator;

class ArrayBuffer {
 public:
  ArrayBuffer(v8::Handle<v8::ArrayBuffer> handle,
              void* data, size_t length)
      : handle_(v8::Isolate::GetCurrent(), handle),
        data_(data),
        length_(length) {
    handle_.SetWeak(this, Destroy);
    handle->SetAlignedPointerInInternalField(0, this);
  }

  static void Destroy(
      const v8::WeakCallbackData<v8::ArrayBuffer, ArrayBuffer>& data) {
    ArrayBuffer* array_buffer = data.GetParameter();
    array_buffer_allocator->Free(array_buffer->data_, array_buffer->length_);
    delete array_buffer;
  }

  void* data() const { return data_; }
  size_t length() const { return length_; }

  static ArrayBuffer* Get(v8::Handle<v8::ArrayBuffer> value) {
    if (value->IsExternal()) {
      return static_cast<ArrayBuffer*>(
          value->GetAlignedPointerFromInternalField(0));
    } else {
      v8::ArrayBuffer::Contents contents(value->Externalize());
      return new ArrayBuffer(value, contents.Data(), contents.ByteLength());
    }
  }

 private:
  v8::Persistent<v8::ArrayBuffer> handle_;
  void* data_;
  size_t length_;
};

}

Object Variant::MakeArrayBuffer(const void* data_in, size_t length) {
  void* data = array_buffer_allocator->AllocateUninitialized(length);
  std::copy(static_cast<const char*>(data_in),
            static_cast<const char*>(data_in) + length,
            static_cast<char*>(data));
  v8::Handle<v8::ArrayBuffer> value =
      v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), data, length);
  new ArrayBuffer(value, data, length);
  return value;
}

Object Variant::MakeArrayBuffer(size_t length) {
  void* data = array_buffer_allocator->Allocate(length);
  v8::Handle<v8::ArrayBuffer> value =
      v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), data, length);
  new ArrayBuffer(value, data, length);
  return value;
}

void* Variant::ExtractArrayBufferData() const {
  v8::Handle<v8::ArrayBuffer> value;
  size_t offset;

  if (handle_->IsArrayBuffer()) {
    value = v8::Handle<v8::ArrayBuffer>::Cast(handle_);
    offset = 0;
  } else if (handle_->IsArrayBufferView()) {
    v8::ArrayBufferView* view = v8::ArrayBufferView::Cast(*handle_);
    value = view->Buffer();
    offset = view->ByteOffset();
  } else {
    throw TypeError("Object is not ArrayBuffer or ArrayBufferView");
  }

  return static_cast<char*>(ArrayBuffer::Get(value)->data()) + offset;
}

size_t Variant::ExtractArrayBufferLength() const {
  if (handle_->IsArrayBuffer())
    return v8::ArrayBuffer::Cast(*handle_)->ByteLength();
  else if (handle_->IsArrayBufferView())
    return v8::ArrayBufferView::Cast(*handle_)->ByteLength();
  else
    throw TypeError("Object is not ArrayBuffer or ArrayBufferView");
}

namespace {

template<typename V8Type, unsigned ElementSize>
Object MakeTypedArray(Variant array_buffer, size_t byte_offset, size_t length) {
  if (array_buffer.IsEmpty())
    array_buffer = Variant::MakeArrayBuffer(byte_offset + ElementSize * length);
  else if (!array_buffer.IsArrayBuffer())
    throw TypeError("Object is not ArrayBuffer");

  size_t array_buffer_length = array_buffer.ExtractArrayBufferLength();

  if (length == static_cast<size_t>(-1))
    length = array_buffer_length / ElementSize;
  else if (byte_offset + ElementSize * length > array_buffer_length)
    throw RangeError("requested ArrayBufferView extend beyond ArrayBuffer");

  return V8Type::New(v8::Handle<v8::ArrayBuffer>::Cast(array_buffer.handle()),
                     byte_offset, length);
}

} // namespace

Object Variant::MakeUint8Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Uint8Array, 1>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeUint8ClampedArray(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Uint8ClampedArray, 1>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeInt8Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Int8Array, 1>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeUint16Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Uint16Array, 2>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeInt16Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Int16Array, 2>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeUint32Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Uint32Array, 4>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeInt32Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Int32Array, 4>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeFloat32Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Float32Array, 4>(
      array_buffer, byte_offset, length);
}

Object Variant::MakeFloat64Array(
    Variant array_buffer, size_t byte_offset, size_t length) {
  return MakeTypedArray<v8::Float64Array, 8>(
      array_buffer, byte_offset, length);
}

Optional<Variant> Variant::FromJSON(std::string json_string) {
  base::Object json(base::Object::GlobalObject().Get("JSON").AsObject());

  v8::TryCatch try_catch;

  Variant result = json.Call("parse", { json_string });
  if (try_catch.HasCaught())
    return Optional<Variant>();
  else
    return Optional<Variant>(result);
}

Variant Variant::ToBoolean() const {
  return Boolean(AsBoolean());
}

Variant Variant::ToNumber() const {
  return Number(AsNumber());
}

Variant Variant::ToInteger() const {
  return Integer(AsInteger());
}

Variant Variant::ToInt32() const {
  return Int32(AsInt32());
}

Variant Variant::ToUInt32() const {
  return UInt32(AsUInt32());
}

Variant Variant::ToString() const {
  return String(AsString());
}

Variant Variant::ToObject() const {
  return Object(AsObject());
}

Variant Variant::Undefined() {
  Variant result;
  result.handle_ = v8::Undefined(CurrentIsolate());
  return result;
}

Variant Variant::Null() {
  Variant result;
  result.handle_ = v8::Null(CurrentIsolate());
  return result;
}

Variant Variant::Boolean(bool value) {
  Variant result;
  result.handle_ = v8::Boolean::New(CurrentIsolate(), value);
  return result;
}

Variant Variant::Number(double value) {
  Variant result;
  result.handle_ = v8::Number::New(CurrentIsolate(), value);
  return result;
}

Variant Variant::Integer(std::int64_t value) {
  Variant result;
  result.handle_ = v8::Number::New(CurrentIsolate(), value);
  return result;
}

Variant Variant::Int32(std::int32_t value) {
  Variant result;
  result.handle_ = v8::Integer::New(CurrentIsolate(), value);
  return result;
}

Variant Variant::UInt32(std::uint32_t value) {
  Variant result;
  result.handle_ = v8::Integer::NewFromUnsigned(CurrentIsolate(), value);
  return result;
}

Variant Variant::String(std::string value) {
  Variant result;
  result.handle_ = base::String::New(value.c_str(), value.length());
  return result;
}

Variant Variant::Object(base::Object value) {
  if (value.IsEmpty())
    return Null();
  else
    return Variant(value.handle());
}

void Variant::SetBoolean(bool value) {
  handle_ = v8::Boolean::New(CurrentIsolate(), value);
}

void Variant::SetNumber(double value) {
  handle_ = v8::Number::New(CurrentIsolate(), value);
}

void Variant::SetInteger(std::int64_t value) {
  handle_ = v8::Number::New(CurrentIsolate(), value);
}

void Variant::SetInt32(std::int32_t value) {
  handle_ = v8::Integer::New(CurrentIsolate(), value);
}

void Variant::SetUInt32(std::uint32_t value) {
  handle_ = v8::Integer::NewFromUnsigned(CurrentIsolate(), value);
}

void Variant::SetString(std::string value) {
  handle_ = base::String::New(value.c_str(), value.length());
}

void Variant::SetObject(base::Object value) {
  handle_ = value.handle_;
}

void Variant::PreInitialize(v8::Isolate::CreateParams& create_params) {
  array_buffer_allocator = new ArrayBufferAllocator;
  create_params.array_buffer_allocator = array_buffer_allocator;
}

}
