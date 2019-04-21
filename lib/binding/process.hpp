#pragma once

#include <nan.h>
#include <sys/types.h>
#include <string>
#include <vector>

using namespace std;

namespace StatsGenius {
struct Process {
    pid_t pid;
    string command_name;
    string command;
    string icon;

    float percent_cpu;
};

NAN_METHOD(V8GetProcesses);
}