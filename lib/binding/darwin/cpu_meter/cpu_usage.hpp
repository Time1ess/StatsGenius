#pragma once

#include <nan.h>

namespace StatsGenius {
namespace CPUMeter {

class CPUUsage {
 public:
  CPUUsage(unsigned int, unsigned int, unsigned int);
  v8::Local<v8::Object> ToV8Object();

  const unsigned int user_cpu_ticks, system_cpu_ticks, idle_cpu_ticks;
};

}  // namespace CPUMeter
}  // namespace StatsGenius