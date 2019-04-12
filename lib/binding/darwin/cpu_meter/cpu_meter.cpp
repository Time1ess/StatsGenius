#include <nan.h>
#include <memory>

#include "cpu_statistics.hpp"

using namespace MacGenius::CPUMeter;

NAN_METHOD(GetCPUStatistics) {
  unique_ptr<CPUStatistics> cpuStatistics = CPUStatistics::Get();
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  if (cpuStatistics) {
    cpuStatistics->UpdateV8ObjectWithThis(result);
  }

  info.GetReturnValue().Set(result);
}

void CPUMeterInit(v8::Local<v8::Object> exports) {
  CPUStatistics::Initialize();
  exports->Set(Nan::New("GetCPUStatistics").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(GetCPUStatistics)->GetFunction());
}

NODE_MODULE(CPUMeter, CPUMeterInit)