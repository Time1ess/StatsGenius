#include <libproc.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "global.hpp"
#include "process.hpp"

using namespace std;

namespace MacGenius {
namespace Darwin {
static unordered_map<int, DarwinProcess> processes;
static size_t cpu_count_;
static uint64_t global_diff_;
static size_t max_argument_length{0};  // Manually set.

size_t GetMaxArgumentLength() {
  static const size_t MAX_ARGUMENT_LENGTH_UNSET = 0;
  if (max_argument_length != MAX_ARGUMENT_LENGTH_UNSET) {
    return max_argument_length;
  }
  size_t size = sizeof(size_t);

  /* Get the maximum process arguments size. */
  static int mib[2] = {CTL_KERN, KERN_ARGMAX};
  if (sysctl(mib, 2, &max_argument_length, &size, nullptr, 0) == -1) {
    throw runtime_error{"Failed to get maximum bytes of argument."};
  }
  return max_argument_length;
}

string GetCommandFromProc(const kinfo_proc* k) {
  size_t size = GetMaxArgumentLength();
  static vector<char> procargs;
  procargs.resize(size);

  int mib[3] = {CTL_KERN, KERN_PROCARGS2, k->kp_proc.p_pid};
  if (sysctl(mib, 3, procargs.data(), &size, nullptr, 0) == -1) {
    return string(k->kp_proc.p_comm);
  }

  return string(procargs.data() + sizeof(int));
}

void DarwinProcess::UpdateWith(kinfo_proc& new_p) {
  if (pre_existing_) {
  }
  pre_existing_ = true;
}

DarwinProcess::DarwinProcess(kinfo_proc& p) : raw_proc_{p} {
  pre_existing_ = false;
}

const kinfo_proc& DarwinProcess::GetRawProc() const { return raw_proc_; }

float DarwinProcess::GetPercentCPU() {
  proc_taskinfo pti;
  if (sizeof(pti) == proc_pidinfo(raw_proc_.kp_proc.p_pid, PROC_PIDTASKINFO, 0,
                                  &pti, sizeof(pti))) {
    if (0 != utime_ || 0 != stime_) {
      uint64_t diff =
          (pti.pti_total_system - stime_) + (pti.pti_total_user - utime_);

      if (global_diff_ != 0) {
        // Make sure that two immediate calls will not make percent_cpu_
        // explosed.
        percent_cpu_ = 1.0 * diff * cpu_count_ / (global_diff_ * 100000.0);
      }
    }

    utime_ = pti.pti_total_system;
    stime_ = pti.pti_total_user;
  }
  return percent_cpu_;
}

Process DarwinProcess::GetProcess() {
  Process p;
  p.pid = raw_proc_.kp_proc.p_pid;
  p.command = GetCommand();
  p.command_name = GetCommandName();
  p.icon = "";
  p.percent_cpu = GetPercentCPU();
  return p;
}

string DarwinProcess::GetCommand() {
  if (command_.empty()) {
    command_ = GetCommandFromProc(&raw_proc_);
  }
  return command_;
}

string DarwinProcess::GetCommandName() {
  if (command_name_.empty()) {
    const string& command = GetCommand();
    auto it = command.rfind('/');
    if (it == string::npos) {
      command_name_ = string(raw_proc_.kp_proc.p_comm);
    } else {
      command_name_ = string(command.begin() + it + 1, command.end());
    }
  }
  return command_name_;
}

vector<Process> GetProcesses() {
  // Update global state
  {
    lock_guard<mutex> guard(data_lock);
    cpu_count_ = cpu_count;
    global_diff_ = global_diff;
  }

  vector<kinfo_proc> procs;
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
  size_t count = 0;
  if (sysctl(mib, 4, nullptr, &count, nullptr, 0) < 0) {
    throw runtime_error{"Unable to get size of kproc_infos."};
  }
  procs.resize(count / sizeof(kinfo_proc));

  if (sysctl(mib, 4, static_cast<void*>(procs.data()), &count, nullptr, 0) <
      0) {
    throw runtime_error{"Unable to get kinfo_procs."};
  }

  unordered_set<int> new_pids;
  for (kinfo_proc& p : procs) {
    auto pid = p.kp_proc.p_pid;
    if (pid < 1) {
      continue;
    }
    new_pids.insert(pid);
    auto it = processes.find(pid);
    if (it != processes.end()) {
      if (strcmp(it->second.GetRawProc().kp_proc.p_comm, p.kp_proc.p_comm) !=
          0) {
        // Replace
        it->second = DarwinProcess(p);
        continue;
      }
      // Update
      it->second.UpdateWith(p);
      continue;
    }
    // Insert
    processes.insert({pid, DarwinProcess(p)});
  }
  // Cleanup
  vector<DarwinProcess*> cands;
  for (auto it = processes.begin(); it != processes.end();) {
    if (new_pids.find(it->first) == new_pids.end()) {
      it = processes.erase(it);
    } else {
      cands.push_back(&it->second);
      it++;
    }
  }

  vector<Process> results;
  for (auto& p : cands) {
    results.push_back(p->GetProcess());
  }
  return results;
}
}  // namespace Darwin
}  // namespace MacGenius
