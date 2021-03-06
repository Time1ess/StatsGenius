#include "cpu_usage.hpp"

namespace StatsGenius {
namespace CPUMeter {
CPUUsage::CPUUsage(unsigned int user_cpu_arg, unsigned int system_cpu_arg,
                   unsigned int idle_cpu_arg)
    : user_cpu_ticks{user_cpu_arg},
      system_cpu_ticks{system_cpu_arg},
      idle_cpu_ticks{idle_cpu_arg} {}

v8::Local<v8::Object> CPUUsage::ToV8Object() {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> user_cpu_prop = Nan::New("userCPU").ToLocalChecked();
  v8::Local<v8::String> system_cpu_prop =
      Nan::New("systemCPU").ToLocalChecked();
  v8::Local<v8::String> idle_cpu_prop = Nan::New("idleCPU").ToLocalChecked();

  const unsigned int total = user_cpu_ticks + system_cpu_ticks + idle_cpu_ticks;

  v8::Local<v8::Value> user_cpu_value =
      Nan::New(100.0 * user_cpu_ticks / total);
  v8::Local<v8::Value> system_cpu_value =
      Nan::New(100.0 * system_cpu_ticks / total);
  v8::Local<v8::Value> idle_cpu_value =
      Nan::New(100.0 * idle_cpu_ticks / total);

  Nan::Set(result, user_cpu_prop, user_cpu_value);
  Nan::Set(result, system_cpu_prop, system_cpu_value);
  Nan::Set(result, idle_cpu_prop, idle_cpu_value);

  return result;
}


}  // namespace CPUMeter
}  // namespace StatsGenius