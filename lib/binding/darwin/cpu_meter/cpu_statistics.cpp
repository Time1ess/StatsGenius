#include <mach/mach.h>
#include <sys/mman.h>
#include <cmath>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "cpu_statistics.hpp"
#include "../third_party/libsmc/smc.hpp"

namespace {
size_t GetCPUInfo(processor_cpu_load_info_t* p) {
  unsigned int num_cpus;
  mach_msg_type_number_t num_infos;
  kern_return_t kern =
      host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &num_cpus,
                          (processor_info_array_t*)p, &num_infos);
  if (kern != KERN_SUCCESS) {
    return -1;
  }
  return num_cpus;
}

void FreeCPULoadInfo(processor_cpu_load_info_t* p) {
  if (nullptr != p && nullptr != *p) {
    if (0 != munmap(*p, vm_page_size)) {
      throw runtime_error{"Failed to free previous cpu load info."};
    }
    *p = nullptr;
  }
}

}  // namespace
namespace StatsGenius {
namespace CPUMeter {

processor_cpu_load_info_t CPUStatistics::prev_cpu_load_info_;
processor_cpu_load_info_t CPUStatistics::cur_cpu_load_info_;
bool CPUStatistics::initialized_;
size_t CPUStatistics::num_cpus_;

void CPUStatistics::Initialize() {
  GetCPUInfo(&prev_cpu_load_info_);
  StatsGenius::Mac::SMC::Initialize();
  initialized_ = true;
}

void CPUStatistics::Destroy() {
  StatsGenius::Mac::SMC::Destroy();
  initialized_ = false;
}

CPUStatistics::CPUStatistics() {}

unique_ptr<CPUStatistics> CPUStatistics::Get() {
  if (!CPUStatistics::initialized_) {
    throw runtime_error{"CPUStatistics has not been initialized"};
  }

  num_cpus_ = GetCPUInfo(&cur_cpu_load_info_);
  auto statistics = make_unique<CPUStatistics>();
  statistics->Prepare();
  return statistics;
}

void CPUStatistics::Prepare() {
  UpdateCPUUsages();
  UpdateAverageLoad();
  UpdateCPUTemperature();
  UpdateUptime();
  processes_ = Process::GetRunningProcesses(num_cpus_, total_usage_.get());
}

void CPUStatistics::UpdateCPUTemperature() {
  // This is slow, with this step, the call time increases from ~2ms to 100+ms.
  // Consider only retrieve this in <Temperature Meter Section>
  vector<pair<string, int>> temps = StatsGenius::Mac::SMC::SMCGetCPUTemperatures();
  float sum = 0;
  for (auto& temp : temps) {
    sum += temp.second;
  }
  cpu_temperature_ = 1.0 * sum / temps.size();
}

void CPUStatistics::UpdateAverageLoad() {
  static int mib[2] = {CTL_VM, VM_LOADAVG};
  loadavg average_load;
  size_t size = sizeof(average_load);
  if (sysctl(mib, 2, &average_load, &size, nullptr, 0) == -1) {
    throw runtime_error{"Failed to get average load."};
  }
  cpu_load_ = make_unique<CPULoad>(average_load);
}

void CPUStatistics::UpdateCPUUsages() {
  unsigned int user_total = 0, sys_total = 0, idle_total = 0, nice_total = 0,
               total = 0;

  for (size_t i = 0; i < num_cpus_; i++) {
    unsigned int user = cur_cpu_load_info_[i].cpu_ticks[CPU_STATE_USER] -
                        prev_cpu_load_info_[i].cpu_ticks[CPU_STATE_USER];
    unsigned int sys = cur_cpu_load_info_[i].cpu_ticks[CPU_STATE_SYSTEM] -
                       prev_cpu_load_info_[i].cpu_ticks[CPU_STATE_SYSTEM];
    unsigned int idle = cur_cpu_load_info_[i].cpu_ticks[CPU_STATE_IDLE] -
                        prev_cpu_load_info_[i].cpu_ticks[CPU_STATE_IDLE];
    unsigned int nice = cur_cpu_load_info_[i].cpu_ticks[CPU_STATE_NICE] -
                        prev_cpu_load_info_[i].cpu_ticks[CPU_STATE_NICE];
    user_total += user;
    sys_total += sys;
    idle_total += idle;
    nice_total += nice;
    core_usage_.push_back(make_unique<CPUUsage>(user, sys, idle));
  }
  // We ingore nice_total here.
  total = user_total + sys_total + idle_total;
  FreeCPULoadInfo(&prev_cpu_load_info_);
  prev_cpu_load_info_ = cur_cpu_load_info_;

  total_usage_ = make_unique<CPUUsage>(user_total, sys_total, idle_total);
}

void CPUStatistics::UpdateUptime() {
  static int mib[2] = {CTL_KERN, KERN_BOOTTIME};
  struct timeval boot_time;
  size_t size = sizeof(boot_time);
  if (sysctl(mib, 2, &boot_time, &size, nullptr, 0) == -1) {
    throw runtime_error{"Failed to get boot time."};
  }
  uptime_ = time(nullptr) - boot_time.tv_sec;
}

v8::Local<v8::Object> CPUStatistics::ToV8ObjectWithThis() {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> total_usage_prop =
      Nan::New("totalUsage").ToLocalChecked();
  v8::Local<v8::Object> total_usage_value = total_usage_->ToV8Object();
  Nan::Set(result, total_usage_prop, total_usage_value);

  v8::Local<v8::String> average_load_prop =
      Nan::New("averageLoad").ToLocalChecked();
  v8::Local<v8::Object> average_load_value = cpu_load_->ToV8Object();
  Nan::Set(result, average_load_prop, average_load_value);

  v8::Local<v8::String> cpu_temperature_prop =
      Nan::New("cpuTemperature").ToLocalChecked();
  v8::Local<v8::Value> cpu_temperature_value = Nan::New(cpu_temperature_);
  Nan::Set(result, cpu_temperature_prop, cpu_temperature_value);

  v8::Local<v8::String> uptime_prop =
      Nan::New("uptime").ToLocalChecked();
  v8::Local<v8::Value> uptime_value = Nan::New(uptime_);
  Nan::Set(result, uptime_prop, uptime_value);

  v8::Local<v8::String> core_usage_prop =
      Nan::New("coreUsage").ToLocalChecked();
  v8::Local<v8::Array> core_usage_value = Nan::New<v8::Array>();
  for (unsigned int i = 0; i < core_usage_.size(); i++) {
    v8::Local<v8::Object> core_usage_value_ = core_usage_[i]->ToV8Object();
    Nan::Set(core_usage_value, i, core_usage_value_);
  }
  Nan::Set(result, core_usage_prop, core_usage_value);

  v8::Local<v8::String> processes_prop = Nan::New("processes").ToLocalChecked();
  v8::Local<v8::Array> processes_value = Nan::New<v8::Array>();
  for (unsigned int i = 0; i < processes_.size(); i++) {
    v8::Local<v8::Object> process_value = processes_[i]->ToV8Object();
    Nan::Set(processes_value, i, process_value);
  }
  Nan::Set(result, processes_prop, processes_value);

  return result;
}
}  // namespace CPUMeter
}  // namespace StatsGenius