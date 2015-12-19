#include "nan.h"
#include "inf.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("parse").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(parse)).ToLocalChecked());

  Set(target, New<String>("verify").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(verify)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
