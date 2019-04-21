#include <nan.h>

#include "cpu.hpp"
#include "process.hpp"

#ifdef __APPLE__
#include "darwin/global.hpp"
using namespace StatsGenius::Darwin;
#elif _WIN32
#elif __linux__
#else
#error No operating system defined
#endif

namespace StatsGenius {
void Init(v8::Local<v8::Object> exports) {
  GlobalInitialize();

  exports->Set(Nan::New("GetProcesses").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(V8GetProcesses)->GetFunction());
  exports->Set(Nan::New("GetAverageLoad").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(V8GetAverageLoad)->GetFunction());
  exports->Set(Nan::New("GetCPUTotalUsage").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(V8GetCPUTotalUsage)->GetFunction());
  exports->Set(Nan::New("GetCPUCoresUsage").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(V8GetCPUCoresUsage)->GetFunction());
  exports->Set(Nan::New("GetCPUCoresTemperature").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(V8GetCPUCoresTemperature)->GetFunction());
  exports->Set(Nan::New("GetUptime").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(V8GetUptime)->GetFunction());

  node::AtExit(GlobalDestroy, nullptr);
}

NODE_MODULE(Genius, Init)

}  // namespace StatsGenius
