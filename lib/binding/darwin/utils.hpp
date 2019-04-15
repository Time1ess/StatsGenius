#pragma once

#include <mach/mach.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdexcept>

using namespace std;

namespace MacGenius {
namespace Darwin {
size_t GetCPUInfo(processor_cpu_load_info_t* p);
void FreeCPULoadInfo(processor_cpu_load_info_t* p);
}  // namespace Darwin
}  // namespace MacGenius