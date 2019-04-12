#pragma once

#include <memory>
#include <string>
#include <vector>
#include <nan.h>
#include <sys/sysctl.h>

using namespace std;

namespace MacGenius {
namespace CPUMeter {
class Process {
 public:
  static vector<unique_ptr<Process>> GetRunningProcesses();
  Process(kinfo_proc proc);
  v8::Local<v8::Object> ToV8Object();

  int GetPid();
  string GetCommand();

 private:
  Process() = default;
  kinfo_proc proc_;
};

}  // namespace CPUMeter
}  // namespace MacGenius