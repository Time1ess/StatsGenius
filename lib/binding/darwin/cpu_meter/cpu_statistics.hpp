#pragma once

#include <mach/mach.h>
#include <sys/sysctl.h>
#include <nan.h>
#include <memory>
#include <vector>

#include "cpu_usage.hpp"
#include "cpu_load.hpp"
#include "process.hpp"

using namespace std;

namespace StatsGenius {
namespace CPUMeter {

class CPUStatistics {
 public:
  static unique_ptr<CPUStatistics> Get();
  CPUStatistics();
  static void Initialize();
  static void Destroy();

  v8::Local<v8::Object> ToV8ObjectWithThis();

 private:
  void Prepare();
  void UpdateCPUUsages();
  void UpdateAverageLoad();
  void UpdateCPUTemperature();
  void UpdateUptime();

  unique_ptr<CPUUsage> total_usage_ {};
  unique_ptr<CPULoad> cpu_load_ {};
  vector<unique_ptr<CPUUsage>> core_usage_;
  float cpu_temperature_ {0};
  vector<const Process*> processes_;
  unsigned int uptime_ {0};

  static size_t num_cpus_;
  static processor_cpu_load_info_t prev_cpu_load_info_, cur_cpu_load_info_;
  static bool initialized_;
};
}  // namespace CPUMeter
}  // namespace StatsGenius