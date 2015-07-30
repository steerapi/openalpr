// openalpr.cc
#include <node.h>
#include <node_buffer.h>
#include <alpr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace openalpr {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Boolean;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Context;
using v8::Function;

using namespace alpr;

bool initialized = false;
static Alpr* nativeAlpr;

void initialize(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  if (args.Length() < 3) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  
  String::Utf8Value v8country(args[0]->ToString());
  String::Utf8Value v8configFile(args[1]->ToString());
  String::Utf8Value v8runtimeDir(args[2]->ToString());
  
  char* ccountry = *v8country;
  char* cconfigFile = *v8configFile;
  char* cruntimeDir = *v8runtimeDir;
  
  // Convert strings from char* to string
  std::string country(ccountry);
  std::string configFile(cconfigFile);
  std::string runtimeDir(cruntimeDir);

  //std::cout << country << std::endl << configFile << std::endl << runtimeDir << std::endl;
  nativeAlpr = new alpr::Alpr(country, configFile, runtimeDir);

  initialized = true;
}

void dispose(const FunctionCallbackInfo<Value>& args)
{
  initialized = false;
  delete nativeAlpr;
}

void isLoaded(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  if (!initialized){
    Local<Boolean> value = Boolean::New(isolate, false);  
    args.GetReturnValue().Set(value);    
  }else{
    Local<Boolean> value = Boolean::New(isolate, nativeAlpr->isLoaded());  
    args.GetReturnValue().Set(value);        
  }
}

void recognizeFile(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  
  String::Utf8Value v8imageFile(args[0]->ToString());
  
  char* cimageFile = *v8imageFile;
  // Convert strings from java to C++ and release resources
  std::string imageFile(cimageFile);

  AlprResults results = nativeAlpr->recognize(imageFile);

  std::string json = Alpr::toJson(results);
  Local<String> value = String::NewFromUtf8(isolate, json.c_str());  
  args.GetReturnValue().Set(value); 
}

void recognizeArray(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  // if (!args[0]->IsString()) {
  //   isolate->ThrowException(Exception::TypeError(
  //       String::NewFromUtf8(isolate, "Wrong arguments")));
  //   return;
  // }
  
  Local<Object> bufferObj = args[0]->ToObject();
  char* buf = node::Buffer::Data(bufferObj);
  int len = node::Buffer::Length(bufferObj);
  
  std::vector<char> cvec(buf, buf+len);

  AlprResults results = nativeAlpr->recognize(cvec);
  std::string json = Alpr::toJson(results);
  
  // Get the global object.
  // Same as using 'global' in Node
  Local<Object> global = isolate->GetCurrentContext()->Global();

  // Get JSON
  // Same as using 'global.JSON'
  Local<Object> JSON = Local<Object>::Cast(
      global->Get(String::NewFromUtf8(isolate,"JSON")));

  // Get stringify
  // Same as using 'global.JSON.stringify'
  Local<Function> parse = Local<Function>::Cast(
      JSON->Get(String::NewFromUtf8(isolate,"parse")));
  
  Local<Value> callargs[1];
  callargs[0] = String::NewFromUtf8(isolate,json.c_str());
  Local<Object> result = Local<Object>::Cast(parse->Call(JSON, 1, callargs));
  
  // Local<String> value = String::NewFromUtf8(isolate, json.c_str());
  args.GetReturnValue().Set(result); 
}

void setDefaultRegion(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  String::Utf8Value v8default_region(args[0]->ToString());
  char* cdefault_region = *v8default_region;
  // Convert strings from java to C++ and release resources
  std::string default_region(cdefault_region);

  nativeAlpr->setDefaultRegion(default_region);
}

void setDetectRegion(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsBoolean()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  bool detect_region = args[0]->BooleanValue();
  nativeAlpr->setDetectRegion(detect_region);
}
    
void setTopN(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  int top_n = args[0]->IntegerValue();
  nativeAlpr->setTopN(top_n);
}

void getVersion(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  std::string version = nativeAlpr->getVersion();

  Local<String> value = String::NewFromUtf8(isolate, version.c_str());  
  args.GetReturnValue().Set(value); 
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "initialize", initialize);
  NODE_SET_METHOD(exports, "dispose", dispose);
  NODE_SET_METHOD(exports, "isLoaded", isLoaded);
  NODE_SET_METHOD(exports, "recognizeFile", recognizeFile);
  NODE_SET_METHOD(exports, "recognizeArray", recognizeArray);
  NODE_SET_METHOD(exports, "setDefaultRegion", setDefaultRegion);
  NODE_SET_METHOD(exports, "setDetectRegion", setDetectRegion);
  NODE_SET_METHOD(exports, "setTopN", setTopN);
  NODE_SET_METHOD(exports, "getVersion", getVersion);
}

NODE_MODULE(addon, init)

}  // namespace openalpr