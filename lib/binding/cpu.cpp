#include "cpu.hpp"

#ifdef __APPLE__
#include "darwin/cpu.hpp"
using namespace StatsGenius::Darwin;
#elif _WIN32
#elif __linux__
#else
#error No operating system defined
#endif

namespace StatsGenius {
NAN_METHOD(V8GetAverageLoad) {
  vector<float> average_load = GetAverageLoad();
  v8::Local<v8::Array> results = Nan::New<v8::Array>();
  for (size_t i = 0; i < average_load.size(); i++) {
    v8::Local<v8::Value> value = Nan::New(average_load[i]);
    Nan::Set(results, i, value);
  }
  info.GetReturnValue().Set(results);
}

NAN_METHOD(V8GetCPUTotalUsage) {
  vector<float> total_usage = GetCPUTotalUsage();
  v8::Local<v8::Array> results = Nan::New<v8::Array>();
  for (size_t i = 0; i < total_usage.size(); i++) {
    v8::Local<v8::Value> value = Nan::New(total_usage[i]);
    Nan::Set(results, i, value);
  }
  info.GetReturnValue().Set(results);
}

NAN_METHOD(V8GetCPUCoresUsage) {
  vector<float> cores_usage = GetCPUCoresUsage();
  v8::Local<v8::Array> results = Nan::New<v8::Array>();
  for (size_t i = 0; i < cores_usage.size(); i++) {
    v8::Local<v8::Value> value = Nan::New(cores_usage[i]);
    Nan::Set(results, i, value);
  }
  info.GetReturnValue().Set(results);
}

NAN_METHOD(V8GetCPUCoresTemperature) {
  vector<int> cores_temp = GetCPUCoresTemperature();
  v8::Local<v8::Array> results = Nan::New<v8::Array>();
  for (size_t i = 0; i < cores_temp.size(); i++) {
    v8::Local<v8::Value> value = Nan::New(cores_temp[i]);
    Nan::Set(results, i, value);
  }
  info.GetReturnValue().Set(results);
}

NAN_METHOD(V8GetUptime) {
  info.GetReturnValue().Set(Nan::New(GetUptime()));
}

}