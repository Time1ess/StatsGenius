#pragma once

#include <vector>

using namespace std;

namespace MacGenius {
vector<float> GetAverageLoad();
vector<float> GetCPUTotalUsage();
vector<float> GetCPUCoresUsage();
vector<float> GetCPUCoresTemperature();
int GetUptime();
}