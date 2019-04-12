#include <sys/sysctl.h>
#include <stdexcept>

#include "process.hpp"

namespace MacGenius {
namespace CPUMeter {
string GetCMDLine(struct kinfo_proc* k, int* basenameOffset) {
  /* This function is from the old Mac version of htop. Originally from ps? */
  int mib[3], argmax, nargs, c = 0;
  size_t size;
  vector<char> procargs;
  string retval;

  /* Get the maximum process arguments size. */
  mib[0] = CTL_KERN;
  mib[1] = KERN_ARGMAX;

  size = sizeof(int);
  if (sysctl(mib, 2, &argmax, &size, NULL, 0) == -1) {
    return string(k->kp_proc.p_comm);
  }

  /* Allocate space for the arguments. */
  procargs.resize(argmax);

  /*
   * Make a sysctl() call to get the raw argument space of the process.
   * The layout is documented in start.s, which is part of the Csu
   * project.  In summary, it looks like:
   *
   * /---------------\ 0x00000000
   * :               :
   * :               :
   * |---------------|
   * | argc          |
   * |---------------|
   * | arg[0]        |
   * |---------------|
   * :               :
   * :               :
   * |---------------|
   * | arg[argc - 1] |
   * |---------------|
   * | 0             |
   * |---------------|
   * | env[0]        |
   * |---------------|
   * :               :
   * :               :
   * |---------------|
   * | env[n]        |
   * |---------------|
   * | 0             |
   * |---------------| <-- Beginning of data returned by sysctl() is here.
   * | argc          |
   * |---------------|
   * | exec_path     |
   * |:::::::::::::::|
   * |               |
   * | String area.  |
   * |               |
   * |---------------| <-- Top of stack.
   * :               :
   * :               :
   * \---------------/ 0xffffffff
   */
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROCARGS2;
  mib[2] = k->kp_proc.p_pid;

  size = static_cast<size_t>(argmax);
  if (sysctl(mib, 3, procargs.data(), &size, NULL, 0) == -1) {
    return string(k->kp_proc.p_comm);
  }
  // procargs now have full information about this command. We need to add some
  // offset to retrieve the full command.

  char* p = procargs.data();
  const char* rp = procargs.data();
  memcpy(&nargs, p, sizeof(nargs));
  p += sizeof(nargs);  // Skip argc

  for (; p < &rp[size]; p++) {
    if (*p == '\0') {
      /* End of exec_path reached. */
      break;
    }
  }
  if (p == &rp[size]) {
    /* End of procargs. */
    return string(k->kp_proc.p_comm);
  }

  for (; p < &rp[size]; p++) {
    if (*p != '\0') {
      /* Beginning of first argument reached. */
      break;
    }
  }
  if (p == &rp[size]) {
    /* End of procargs. */
    return string(k->kp_proc.p_comm);
  }

  char* start = p;
  for (char* lp = nullptr; c < nargs && p < &rp[size]; p++) {
    if (*p == '\0') {
      c++;
      if (lp != nullptr) {
        *lp = ' ';
      }
      lp = p;
      if (*basenameOffset == 0) {
        *basenameOffset = p - start;
      }
    }
  }

  return string(start);
}

void GetProcessesData() {
  size_t count = 0;
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};

  vector<kinfo_proc> processes;

  if (sysctl(mib, 4, nullptr, &count, nullptr, 0) < 0) {
    throw runtime_error{"Unable to get size of kproc_infos."};
  }
  processes.resize(count / sizeof(kinfo_proc));

  if (sysctl(mib, 4, static_cast<void*>(processes.data()), &count, nullptr, 0) <
      0) {
    throw runtime_error{"Unable to get kinfo_procs."};
  }

  for (kinfo_proc& p : processes) {
    int offset = 0;
    GetCMDLine(&p, &offset);
  }
}

vector<unique_ptr<Process>> Process::GetProcesses() {
  vector<unique_ptr<Process>> processes;

  return processes;
}

}  // namespace CPUMeter
}  // namespace MacGenius