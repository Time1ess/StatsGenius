#pragma once

#include <sys/sysctl.h>
#include <vector>

#include "../process.hpp"

using namespace std;

namespace MacGenius {
namespace Darwin {
class DarwinProcess {
 public:
  DarwinProcess(kinfo_proc&);
  void UpdateWith(kinfo_proc&);
  const kinfo_proc& GetRawProc() const;
  Process GetProcess();

 private:
  string GetCommand();
  float GetPercentCPU();

  bool pre_existing_ {false};
  kinfo_proc raw_proc_ {};
  float percent_cpu_ {0};
  uint64_t utime_ {0};
  uint64_t stime_ {0};
  string command_ {""};

};
vector<Process> GetProcesses();
}  // namespace Darwin
}  // namespace MacGenius