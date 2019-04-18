#include <iostream>
#include <thread>

#include "global.hpp"
#include "third_party/libsmc/smc.hpp"

namespace MacGenius {
namespace Darwin {
processor_cpu_load_info_t prev_load;
processor_cpu_load_info_t curr_load;
size_t cpu_count;
uint64_t global_diff;
vector<uint64_t> cpu_total_ticks;
vector<uint64_t> cpu_core_ticks;
mutex data_lock;
unique_ptr<thread> update_thread;
bool thread_finished{false};

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

void UpdateCPULoadInfo() {
  lock_guard<mutex> guard(data_lock);
  FreeCPULoadInfo(&prev_load);
  prev_load = curr_load;
  GetCPUInfo(&curr_load);

  global_diff = 0;
  cpu_core_ticks.assign(cpu_count * (CPU_STATE_MAX + 1), 0);
  cpu_total_ticks.assign(CPU_STATE_MAX + 1, 0);
  for (size_t i = 0; i < cpu_count; i++) {
    for (size_t j = 0; j < CPU_STATE_MAX; j++) {
      uint64_t diff = curr_load[i].cpu_ticks[j] - prev_load[i].cpu_ticks[j];
      global_diff += diff;
      cpu_total_ticks[j] += diff;
      cpu_total_ticks[CPU_STATE_MAX] += diff;
      cpu_core_ticks[i * (CPU_STATE_MAX + 1) + j] += diff;
      cpu_core_ticks[i * (CPU_STATE_MAX + 1) + CPU_STATE_MAX] += diff;
    }
  }
}

void GlobalInitialize() {
  cpu_count = GetCPUInfo(&curr_load);
  cpu_total_ticks.resize(CPU_STATE_MAX + 1);  // User, Sys, Idle, Nice, Total
  cpu_core_ticks.resize(cpu_count * (CPU_STATE_MAX + 1));
  update_thread = make_unique<thread>([&]() {
    while (true) {
      if (thread_finished) {
        break;
      }
      UpdateCPULoadInfo();
      this_thread::sleep_for(chrono::seconds(2));
    }
  });

  // Initialize libsmc
  SMC::SMCInitialize();
}

void GlobalDestroy(void*) {
  // Destroy libsmc
  SMC::SMCDestroy();

  thread_finished = true;
  update_thread->join();
}
}  // namespace Darwin
}  // namespace MacGenius
