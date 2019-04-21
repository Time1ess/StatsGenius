#pragma once

#include <nan.h>
#include <vector>

using namespace std;

namespace StatsGenius {
NAN_METHOD(V8GetAverageLoad);
NAN_METHOD(V8GetCPUTotalUsage);
NAN_METHOD(V8GetCPUCoresUsage);
NAN_METHOD(V8GetCPUCoresTemperature);
NAN_METHOD(V8GetUptime);
}