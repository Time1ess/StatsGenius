#include <stdexcept>

#include "process.hpp"

namespace {
string GetCommandFromProc(const kinfo_proc* k) {
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

  for (; p < &rp[size] && *p != '\0'; p++);
  if (p == &rp[size]) {
    /* End of procargs. */
    return string(k->kp_proc.p_comm);
  }

  for (; p < &rp[size] && *p == '\0'; p++);
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
    }
  }

  return string(start);
}
}

namespace MacGenius {
namespace CPUMeter {
Process::Process(kinfo_proc proc) : proc_{move(proc)} {}

vector<unique_ptr<Process>> Process::GetRunningProcesses() {
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};

  vector<kinfo_proc> procs;

  size_t count = 0;
  if (sysctl(mib, 4, nullptr, &count, nullptr, 0) < 0) {
    throw runtime_error{"Unable to get size of kproc_infos."};
  }
  procs.resize(count / sizeof(kinfo_proc));

  if (sysctl(mib, 4, static_cast<void*>(procs.data()), &count, nullptr, 0) <
      0) {
    throw runtime_error{"Unable to get kinfo_procs."};
  }

  vector<unique_ptr<Process>> processes;
  int cnt = 0;
  for (kinfo_proc& p : procs) {
    unique_ptr<Process> proc = make_unique<Process>(p);
    processes.push_back(move(proc));
    if (++cnt == 8) {
      break;
    }
  }
  return processes;
}

int Process::GetPid() {
  return proc_.kp_proc.p_pid;
}

string Process::GetCommand() {
  return GetCommandFromProc(&proc_);
}

v8::Local<v8::Object> Process::ToV8Object() {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> pid_prop = Nan::New("pid").ToLocalChecked();
  v8::Local<v8::String> command_prop = Nan::New("command").ToLocalChecked();
  
  v8::Local<v8::Value> pid_value = Nan::New(GetPid());
  v8::Local<v8::Value> command_value = Nan::New(GetCommand()).ToLocalChecked();

  Nan::Set(result, pid_prop, pid_value);
  Nan::Set(result, command_prop, command_value);

  return result;
}

}  // namespace CPUMeter
}  // namespace MacGenius