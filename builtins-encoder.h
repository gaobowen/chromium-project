#ifndef V8_BUILTINS_BUILTINS_ENCODER_H_
#define V8_BUILTINS_BUILTINS_ENCODER_H_

#include "src/builtins/builtins.h"
#include "src/builtins/builtins-utils-inl.h"

namespace v8 {
  namespace internal {

    //提取字段
    inline Handle<Object> EncodeGetProperty(v8::internal::Isolate* isolate, v8::internal::Handle<v8::internal::Object> object, const char* name) {
      Handle<String> h_name = isolate->factory()->InternalizeUtf8String(name);
      Handle<Object> result;
      bool succ = Object::GetProperty(isolate, object, h_name).ToHandle(&result);
      if (result.is_null() || !succ) {
        return isolate->factory()->undefined_value();
      }
      return result;
    }

    //对象转cstring
    inline std::unique_ptr<char[]> EncodeToCString(v8::internal::Isolate* isolate, v8::internal::Handle<v8::internal::Object> object) {
      if (!object->IsString()) return std::unique_ptr<char[]>(nullptr);
      Handle<String> string = Handle<String>::cast(object);
      return string->ToCString();
    }

    //TypedArray 或 ArrayBuffer 对象转指针
    inline std::tuple<uint8_t*, size_t> EncodeToBuffer(v8::internal::Isolate* isolate, v8::internal::Handle<v8::internal::Object> object) {
      Handle<JSArrayBuffer> jsab;
      //size_t size;
      if (object->IsJSTypedArray()) {
        auto jsta = JSTypedArray::Validate(isolate, object, "EncodeToBuffer").ToHandleChecked();
        jsab = jsta->GetBuffer();
        //size = jsta->GetByteLength();
      }
      else if (object->IsJSArrayBuffer()) {
        jsab = Handle<JSArrayBuffer>::cast(object);
        if (jsab->IsNullOrUndefined()) {
          isolate->Throw(*isolate->factory()->NewTypeError(
            MessageTemplate::kInvalidArrayBufferLength));
        }
      }
      if (jsab->IsNullOrUndefined()) return std::tuple<uint8_t*, size_t>(nullptr, 0);
      std::shared_ptr<v8::internal::BackingStore> store = jsab->GetBackingStore();
      return std::tuple<uint8_t*, size_t>((uint8_t*)store->buffer_start(), store->byte_length());
    }

    inline Handle<JSObject> EncodeNewJSObject(v8::internal::Isolate* isolate) {
      return isolate->factory()->NewJSObject(isolate->object_function(), AllocationType::kOld);
    }

    inline void EncodeAddPropertyObject(Isolate* isolate, Handle<JSObject> object, const char* key, Handle<Object> value) {
      JSObject::AddProperty(isolate, object, key, value, PropertyAttributes::NONE);
    }

    inline void EncodeAddPropertyNumber(Isolate* isolate, Handle<JSObject> object, const char* key, double value) {
      Handle<Object> valueObj = isolate->factory()->NewNumber(value);
      JSObject::AddProperty(isolate, object, key, valueObj, PropertyAttributes::NONE);
    }

    inline void EncodeAddPropertyString(Isolate* isolate, Handle<JSObject> object, const char* key, const char* value) {
      Handle<String> strObj = isolate->factory()->InternalizeUtf8String(value);
      //字段设置为 PropertyAttributes::DontEnum 时，无法通过键值对遍历 Object.keys(retObj)
      JSObject::AddProperty(isolate, object, key, strObj, PropertyAttributes::NONE);
    }

    inline Handle<JSArrayBuffer> EncodeNewJSArrayBuffer(v8::internal::Isolate* isolate, size_t size) {
      MaybeHandle<JSArrayBuffer> arrbuffOjb =
        isolate->factory()->NewJSArrayBufferAndBackingStore(
          size, InitializedFlag::kZeroInitialized);
      Handle<JSArrayBuffer> result;
      if (!arrbuffOjb.ToHandle(&result)) {
        isolate->Throw(*isolate->factory()->NewTypeError(
          MessageTemplate::kInvalidArrayBufferLength));
        return result;
      }
      return result;
    }

  }  // namespace internal
}  // namespace v8

#endif  // V8_BUILTINS_BUILTINS_ENCODER_H_
