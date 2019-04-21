#pragma once

#include <mach/mach.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdexcept>
#include <mutex>
#include <vector>


using namespace std;

namespace StatsGenius {
namespace Darwin {

extern mutex data_lock;
extern processor_cpu_load_info_t prev_load;
extern processor_cpu_load_info_t curr_load;
extern size_t cpu_count;
extern uint64_t global_diff;
extern vector<uint64_t> cpu_total_ticks;
extern vector<uint64_t> cpu_core_ticks;

void GlobalInitialize();
void GlobalDestroy(void*);
}  // namespace Darwin
}  // namespace StatsGenius