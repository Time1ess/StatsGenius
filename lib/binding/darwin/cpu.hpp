#pragma once

#include <vector>
#include <cstdint>

using namespace std;

namespace MacGenius {
namespace Darwin {
vector<float> GetAverageLoad();
vector<float> GetCPUTotalUsage();
vector<float> GetCPUCoresUsage();
vector<int> GetCPUCoresTemperature();
uint32_t GetUptime();
}  // namespace Darwin
}  // namespace MacGenius