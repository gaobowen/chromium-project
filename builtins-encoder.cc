#include "src/builtins/builtins-encoder.h"



#include <stdio.h>
#include "src/builtins/builtins.h"
#include "src/builtins/builtins-utils-inl.h"


namespace v8 {
  namespace internal {

    BUILTIN(Encode) {
      HandleScope scope(isolate);
      Handle<Object> arg1 = args.atOrUndefined(isolate, 1);
      Handle<Object> arg2 = args.atOrUndefined(isolate, 2);
      Handle<Object> arg3 = args.atOrUndefined(isolate, 3);
      Handle<Object> arg4 = args.atOrUndefined(isolate, 4);

      //------------------------JS类型 转 C/C++类型--------------------------------//
      //解析对象
      Handle<Object> arg1_aaaObj = EncodeGetProperty(isolate, arg1, "aaa");
      //解析字符串
      std::unique_ptr<char[]> aaa_u_ptr = EncodeToCString(isolate, arg1_aaaObj);
      printf("arg1.aaa=%s\n", aaa_u_ptr.get());
      Handle<Object> arg1_bbbObj = EncodeGetProperty(isolate, arg1, "bbb");
      if (arg1_bbbObj->IsUndefined()) {
        printf("arg1.bbb 不存在 \n");
      }

      //解析int32
      int arg2_v = 0;
      arg2->ToInt32(&arg2_v);
      printf("arg2=%d\n", arg2_v);

      //解析double
      double arg3_v = arg3->Number();
      printf("arg3=%lf\n", arg3_v);

      //解析TypedArray和ArrayBuffer
      std::tuple<uint8_t*, size_t> arg4_v = EncodeToBuffer(isolate, arg4);
      auto buffer_ptr = std::get<0>(arg4_v);
      auto buffer_size = std::get<1>(arg4_v);
      printf("arg4=[");
      for (size_t i = 0; i < buffer_size; i++)
      {
        printf(" %d ", buffer_ptr[i]);
      }
      printf("]\r\n");
      //-------------------------------------------------------------------------//


      //---------------------------返回JS对象------------------------------------//
      //创建返回对象
      Handle<JSObject> returnObj = EncodeNewJSObject(isolate);

      //添加Number类型 字段
      EncodeAddPropertyNumber(isolate, returnObj, "num", 3.14);

      //添加string类型 字段
      EncodeAddPropertyString(isolate, returnObj, "str", "adcdef");

      //添加 数组 类型 字段
      size_t buffsize = 5;
      auto result = EncodeNewJSArrayBuffer(isolate, buffsize);
      uint8_t* data =
          reinterpret_cast<uint8_t*>(result->GetBackingStore()->buffer_start());
      data[0] = 3;
      data[1] = 2;
      data[2] = 1;
      EncodeAddPropertyObject(isolate, returnObj, "arrbuff", result);

      //-------------------------------------------------------------------------//
      return *returnObj;
    }
    
  } // internal
} // v8
