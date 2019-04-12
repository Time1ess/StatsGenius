#include <mach/mach.h>
#include <sys/mman.h>
#include <cmath>
#include <memory>
#include <stdexcept>

#include "cpu_statistics.hpp"

namespace {
unsigned int GetCPUInfo(processor_cpu_load_info_t* p) {
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
namespace MacGenius {
namespace CPUMeter {

processor_cpu_load_info_t CPUStatistics::prev_cpu_load_info_;
processor_cpu_load_info_t CPUStatistics::cur_cpu_load_info_;
bool CPUStatistics::initialized_;

void CPUStatistics::Initialize() {
  GetCPUInfo(&prev_cpu_load_info_);
  initialized_ = true;
}

unique_ptr<CPUStatistics> CPUStatistics::Get() {
  if (!CPUStatistics::initialized_) {
    throw runtime_error{"CPUStatistics has not been initialized"};
  }
  unsigned int num_cpus;

  num_cpus = GetCPUInfo(&cur_cpu_load_info_);

  unsigned int user_total = 0, sys_total = 0, idle_total = 0, nice_total = 0,
               total = 0;
  vector<unique_ptr<CPUUsage>> core_usage;

  for (unsigned int i = 0; i < num_cpus; i++) {
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
    core_usage.push_back(make_unique<CPUUsage>(user, sys, idle));
  }
  // We ingore nice_total here.
  total = user_total + sys_total + idle_total;
  if (fabs(total) < 1e-5) {
    return {};
  }
  FreeCPULoadInfo(&prev_cpu_load_info_);
  prev_cpu_load_info_ = cur_cpu_load_info_;

  return make_unique<CPUStatistics>(
      make_unique<CPUUsage>(user_total, sys_total, idle_total),
      move(core_usage), Process::GetProcesses());
}

CPUStatistics::CPUStatistics(unique_ptr<CPUUsage> total_usage,
                             vector<unique_ptr<CPUUsage>> core_usage,
                             vector<unique_ptr<Process>> processes)
    : total_usage_{move(total_usage)},
      core_usage_{move(core_usage)},
      processes_{move(processes)} {}

void CPUStatistics::UpdateV8ObjectWithThis(v8::Local<v8::Object>& result) {
  v8::Local<v8::String> total_usage_prop =
      Nan::New("totalUsage").ToLocalChecked();
  v8::Local<v8::String> core_usage_prop =
      Nan::New("coreUsage").ToLocalChecked();

  v8::Local<v8::Object> total_usage_value = total_usage_->ToV8Object();
  v8::Local<v8::Array> core_usage_value = Nan::New<v8::Array>();
  for (unsigned int i = 0; i < core_usage_.size(); i++) {
    v8::Local<v8::Object> core_usage_value_ = core_usage_[i]->ToV8Object();
    Nan::Set(core_usage_value, i, core_usage_value_);
  }

  Nan::Set(result, total_usage_prop, total_usage_value);
  Nan::Set(result, core_usage_prop, core_usage_value);
}
}  // namespace CPUMeter
}  // namespace MacGenius