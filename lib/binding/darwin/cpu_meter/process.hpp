#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace MacGenius {
namespace CPUMeter {
class Process {
 public:
  static vector<unique_ptr<Process>> GetProcesses();

 private:
  Process() = default;
};

}  // namespace CPUMeter
}  // namespace MacGenius