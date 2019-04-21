#pragma once

#include <memory>
#include <string>
#include <vector>
#include <nan.h>
#include <sys/sysctl.h>
#include <libproc.h>

#include "cpu_usage.hpp"

using namespace std;

namespace StatsGenius {
namespace CPUMeter {


class Process {
 public:
  static vector<const Process*> GetRunningProcesses(int num_cpus, const CPUUsage* total_usage, size_t max_num = 8);
  Process(kinfo_proc proc);
  void Prepare();
  void Update();

  v8::Local<v8::Object> ToV8Object() const;
  int GetPid() const;
  string GetCommandName() const;
  string GetCommand() const;
  string GetBase64Icon() const;
  float GetPercentCPU() const;

 private:
  Process() = default;
  string command_ {""};
  kinfo_proc proc_;
  unsigned int utime_ {0};
  unsigned int stime_ {0};
  float percent_cpu_ {0};

  static int num_cpus_;
  static CPUUsage const* total_usage_;
};

}  // namespace CPUMeter
}  // namespace StatsGenius