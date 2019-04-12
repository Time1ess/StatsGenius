#pragma once

#include <nan.h>

namespace MacGenius {
namespace CPUMeter {

class CPUUsage {
 public:
  CPUUsage(unsigned int, unsigned int, unsigned int);
  v8::Local<v8::Object> ToV8Object();

 private:
  unsigned int user_cpu_ticks_, system_cpu_ticks_, idle_cpu_ticks_;
};

}  // namespace CPUMeter
}  // namespace MacGenius