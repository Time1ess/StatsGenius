#include <mach/mach.h>
#include <nan.h>
#include <sys/mman.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std;

static processor_cpu_load_info_t prev_load_, cur_load_;

struct CPUUsage {
  unsigned int user_cpu, system_cpu, idle_cpu, total_cpu;

  CPUUsage(unsigned int user_cpu_arg, unsigned int system_cpu_arg,
           unsigned int idle_cpu_arg, unsigned int total_cpu_arg)
      : user_cpu{user_cpu_arg},
        system_cpu{system_cpu_arg},
        idle_cpu{idle_cpu_arg},
        total_cpu{total_cpu_arg} {}
};

struct CPUData {
  CPUUsage total_usage;
  vector<CPUUsage> core_usage;

  CPUData(CPUUsage&& total_usage_arg, vector<CPUUsage>&& core_usage_arg)
      : total_usage{total_usage_arg}, core_usage{core_usage_arg} {}
};

unsigned int GetCPULoadInfo(processor_cpu_load_info_t* p) {
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

v8::Local<v8::Object> GenerateV8ObjectForCPUUsage(CPUUsage&& usage) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> user_cpu_prop = Nan::New("userCPU").ToLocalChecked();
  v8::Local<v8::String> system_cpu_prop =
      Nan::New("systemCPU").ToLocalChecked();
  v8::Local<v8::String> idle_cpu_prop = Nan::New("idleCPU").ToLocalChecked();

  const unsigned int total = usage.user_cpu + usage.system_cpu + usage.idle_cpu;

  v8::Local<v8::Value> user_cpu_value = Nan::New(100.0 * usage.user_cpu / total);
  v8::Local<v8::Value> system_cpu_value =
      Nan::New(100.0 * usage.system_cpu / total);
  v8::Local<v8::Value> idle_cpu_value = Nan::New(100.0 * usage.idle_cpu / total);

  Nan::Set(result, user_cpu_prop, user_cpu_value);
  Nan::Set(result, system_cpu_prop, system_cpu_value);
  Nan::Set(result, idle_cpu_prop, idle_cpu_value);

  return result;
}

v8::Local<v8::Object> GenerateV8ObjectForCPUData(CPUData&& data) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> total_usage_prop =
      Nan::New("totalUsage").ToLocalChecked();
  v8::Local<v8::String> core_usage_prop =
      Nan::New("coreUsage").ToLocalChecked();

  v8::Local<v8::Object> total_usage_value =
      GenerateV8ObjectForCPUUsage(move(data.total_usage));
  v8::Local<v8::Array> core_usage_value = Nan::New<v8::Array>();
  for (unsigned int i = 0; i < data.core_usage.size(); i++) {
    v8::Local<v8::Object> core_usage_value_ =
        GenerateV8ObjectForCPUUsage(move(data.core_usage[i]));
    Nan::Set(core_usage_value, i, core_usage_value_);
  }

  Nan::Set(result, total_usage_prop, total_usage_value);
  Nan::Set(result, core_usage_prop, core_usage_value);

  return result;
}

NAN_METHOD(GetCPULoad) {
  unsigned int num_cpus;

  num_cpus = GetCPULoadInfo(&cur_load_);

  unsigned int user_total = 0, sys_total = 0, idle_total = 0, nice_total = 0,
               total = 0;
  vector<CPUUsage> cores_load;

  for (unsigned int i = 0; i < num_cpus; i++) {
    const unsigned int user = cur_load_[i].cpu_ticks[CPU_STATE_USER] -
                              prev_load_[i].cpu_ticks[CPU_STATE_USER];
    const unsigned int sys = cur_load_[i].cpu_ticks[CPU_STATE_SYSTEM] -
                             prev_load_[i].cpu_ticks[CPU_STATE_SYSTEM];
    const unsigned int idle = cur_load_[i].cpu_ticks[CPU_STATE_IDLE] -
                              prev_load_[i].cpu_ticks[CPU_STATE_IDLE];
    const unsigned int nice = cur_load_[i].cpu_ticks[CPU_STATE_NICE] -
                              prev_load_[i].cpu_ticks[CPU_STATE_NICE];
    user_total += user;
    sys_total += sys;
    idle_total += idle;
    nice_total += nice;
    cores_load.push_back({user, sys, idle, user + sys + idle + nice});
  }
  total = user_total + sys_total + idle_total + nice_total;
  if (fabs(total) < 1e-5) {
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  FreeCPULoadInfo(&prev_load_);
  prev_load_ = cur_load_;

  CPUData data(CPUUsage(user_total, sys_total, idle_total, total),
               move(cores_load));

  auto res = GenerateV8ObjectForCPUData(move(data));
  info.GetReturnValue().Set(res);
}

void CPUMeterInit(v8::Local<v8::Object> exports) {
  GetCPULoadInfo(&prev_load_);
  exports->Set(Nan::New("GetCPULoadInfo").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(GetCPULoad)->GetFunction());
}

NODE_MODULE(CPUMeter, CPUMeterInit)