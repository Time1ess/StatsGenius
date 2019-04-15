#pragma once

#include <sys/types.h>
#include <string>
#include <vector>

using namespace std;

namespace MacGenius {
struct Process {
    pid_t pid;
    string command_name;
    string command;
    string icon;

    float percent_cpu;
};

void ExportedGetProcesses();
}