#include <nan.h>
#include <memory>

#include "cpu_statistics.hpp"

using namespace StatsGenius::CPUMeter;

NAN_METHOD(GetCPUStatistics) {
  unique_ptr<CPUStatistics> cpuStatistics = CPUStatistics::Get();
  if (!cpuStatistics) {
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  auto result = cpuStatistics->ToV8ObjectWithThis();
  info.GetReturnValue().Set(result);
}

void CPUMeterInit(v8::Local<v8::Object> exports) {
  CPUStatistics::Initialize();
  exports->Set(Nan::New("GetCPUStatistics").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(GetCPUStatistics)->GetFunction());
}

NODE_MODULE(CPUMeter, CPUMeterInit)