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
  base::Object global(CurrentContext()->Global());
  base::Object json(global.Get("JSON").AsObject());

  return json.Call("stringify", { *this }).AsString();
}

Optional<Variant> Variant::FromJSON(std::string json_string) {
  base::Object global(CurrentContext()->Global());
  base::Object json(global.Get("JSON").AsObject());

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

void Variant::Initialize() {
}

}
