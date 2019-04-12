#pragma once

#include <mach/mach.h>
#include <nan.h>
#include <memory>
#include <vector>

#include "cpu_usage.hpp"
#include "process.hpp"

using namespace std;

namespace MacGenius {
namespace CPUMeter {

class CPUStatistics {
 public:
  static unique_ptr<CPUStatistics> Get();
  CPUStatistics(unique_ptr<CPUUsage>, vector<unique_ptr<CPUUsage>>, vector<unique_ptr<Process>>);
  static void Initialize();

  void UpdateV8ObjectWithThis(v8::Local<v8::Object>& result);

 private:
  unique_ptr<CPUUsage> total_usage_ {};
  vector<unique_ptr<CPUUsage>> core_usage_;
  vector<unique_ptr<Process>> processes_;

  static processor_cpu_load_info_t prev_cpu_load_info_, cur_cpu_load_info_;
  static bool initialized_;
};
}  // namespace CPUMeter
}  // namespace MacGenius