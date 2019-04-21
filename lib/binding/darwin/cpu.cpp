#include <sys/sysctl.h>
#include <iostream>
#include <mutex>
#include <stdexcept>

#include "cpu.hpp"
#include "global.hpp"
#include "third_party/libsmc/smc.hpp"

namespace StatsGenius {
namespace Darwin {

vector<float> GetAverageLoad() {
  static int mib[2] = {CTL_VM, VM_LOADAVG};
  loadavg average_load;
  size_t size = sizeof(average_load);
  if (sysctl(mib, 2, &average_load, &size, nullptr, 0) == -1) {
    throw runtime_error{"Failed to get average load."};
  }
  vector<float> res(3);  // last 1m, last 5m, last 15m
  for (size_t i = 0; i < 3; i++) {
    res[i] = 1.0 * average_load.ldavg[i] / average_load.fscale;
  }
  return res;
}

vector<float> GetCPUTotalUsage() {
  vector<float> res;
  {
    lock_guard<mutex> guard(data_lock);
    res.resize(cpu_total_ticks.size());
    for (size_t i = 0; i < cpu_total_ticks.size(); i++) {
      res[i] = 1.0 * cpu_total_ticks[i] / cpu_total_ticks[CPU_STATE_MAX];
    }
  }
  return res;
}

vector<float> GetCPUCoresUsage() {
  vector<float> res;
  {
    lock_guard<mutex> guard(data_lock);
    res.resize(cpu_core_ticks.size());
    for (size_t i = 0; i < cpu_count; i++) {
      size_t core_offset = i * (CPU_STATE_MAX + 1);
      for (size_t j = 0; j < CPU_STATE_MAX; j++) {
        res[core_offset + j] =
            1.0 * cpu_core_ticks[core_offset + j] /
            cpu_core_ticks[core_offset + CPU_STATE_MAX];
      }
    }
  }
  return res;
}

vector<int> GetCPUCoresTemperature() {
  // This is slow, with this step, the call time increases from ~2ms to 100+ms.
  // Consider only retrieve this in <Temperature Meter Section>
  auto temps = SMC::SMCGetCPUTemperatures();
  vector<int> res(temps.size());
  for (size_t i = 0; i < temps.size(); i++) {
    res[i] = temps[i].second;
  }
  return res;
}

uint32_t GetUptime() {
  static int mib[2] = {CTL_KERN, KERN_BOOTTIME};
  struct timeval boot_time;
  size_t size = sizeof(boot_time);
  if (sysctl(mib, 2, &boot_time, &size, nullptr, 0) == -1) {
    throw runtime_error{"Failed to get boot time."};
  }
  return time(nullptr) - boot_time.tv_sec;
}
}  // namespace Darwin
}  // namespace StatsGenius