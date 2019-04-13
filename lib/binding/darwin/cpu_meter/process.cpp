#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include "process.hpp"

namespace {

static size_t MaxArgumentLength = 0;  // Manually set.

size_t GetMaxArgumentLength() {
  static const size_t MAX_ARGUMENT_LENGTH_UNSET = 0;
  if (MaxArgumentLength != MAX_ARGUMENT_LENGTH_UNSET) {
    return MaxArgumentLength;
  }
  size_t size = sizeof(int);

  /* Get the maximum process arguments size. */
  static int mib[2] = {CTL_KERN, KERN_ARGMAX};
  if (sysctl(mib, 2, &MaxArgumentLength, &size, nullptr, 0) == -1) {
    throw runtime_error{"Failed to get maximum bytes of argument."};
  }
  return MaxArgumentLength;
}

string GetCommandFromProc(const kinfo_proc* k) {
  size_t size = GetMaxArgumentLength();
  vector<char> procargs(size);

  int mib[3] = {CTL_KERN, KERN_PROCARGS2, k->kp_proc.p_pid};
  if (sysctl(mib, 3, procargs.data(), &size, NULL, 0) == -1) {
    return string(k->kp_proc.p_comm);
  }

  return string(procargs.data() + sizeof(int));
}

/*
inline bool IsFileExists(const string& path) {
  struct stat buffer;
  return stat(path.c_str(), &buffer) == 0;
}

string ReadFile(const string& path) {
  std::ifstream t(path);
  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());
  return str;
}

string FindAppIconPath(const string& command) {
  static const string app_suffix{".app"};
  auto it = command.find(app_suffix);
  if (it == string::npos) {
    return "";
  }
  const string base_path = command.substr(0, it + app_suffix.length()) +
"/Contents";

  const string info_plist_path = base_path + "/Info.plist";
  if (!IsFileExists(info_plist_path)) {
    return "";
  }
  const string info_plist_content = ReadFile(info_plist_path);

  regex re {"<key>CFBundleIconFile</key>[\\s\\S]*<string>(.+)</string>"};
  smatch match;
  if (!regex_search(info_plist_content.begin(), info_plist_content.end(), match,
re) or match.size() != 2) { return "";
  }
  string app_icon_path = base_path + "/Resources/" + match.str(1);
  if (!IsFileExists(app_icon_path)) {
    const string app_icon_path_with_ext = app_icon_path + ".icns";
    if (!IsFileExists(app_icon_path_with_ext)) {
      return "";
    }
    return app_icon_path_with_ext;
  }
  return app_icon_path;
}

string ReadAndEncodeAppIcon(const string& path) {
  FILE* in_file {};

        icns_family_t* icon_family {};
  cout << path << endl;
  in_file = fopen(path.c_str(), "r");
  if (in_file == nullptr) {
    cout <<"Open Error" << endl;
    return "";
  }
  int error = icns_read_family_from_file(in_file, &icon_family);
  fclose(in_file);
  if (error != 0) {
    cout <<"Read Error" << endl;
    return "";
  }
  icns_image_t icon_image;

  error = icns_get_image32_with_mask_from_family(icon_family,
ICNS_128X128_32BIT_DATA, &icon_image); if (error != 0) { cout <<"Image Error" <<
endl; return "";
  }
  cout << "Width: " << icon_image.imageWidth << endl;
  cout << "heigth: " << icon_image.imageHeight << endl;
  return "";
}
*/
}  // namespace

namespace MacGenius {
namespace CPUMeter {

static unordered_map<int, unique_ptr<Process>> processes;
int Process::num_cpus_ = 0;
CPUUsage const* Process::total_usage_ {};

Process::Process(kinfo_proc proc)
    : proc_{move(proc)} {
  command_ = GetCommandFromProc(&proc_);
}

vector<const Process*> Process::GetRunningProcesses(int num_cpus,
                                                    const CPUUsage* total_usage,
                                                    int max_num) {

  Process::num_cpus_ = num_cpus;
  Process::total_usage_ = total_usage;
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

  unordered_set<int> new_pids;
  for (kinfo_proc& p : procs) {
    unique_ptr<Process> proc = make_unique<Process>(p);
    new_pids.insert(proc->GetPid());
    auto it = processes.find(proc->GetPid());
    if (it != processes.end()) {
      if (it->second->GetCommand() != proc->GetCommand()) {
        // Replace
        it->second = move(proc);
        continue;
      }
      // Update
      it->second->Update();
      continue;
    }
    // Insert
    processes[proc->GetPid()] = move(proc);
  }
  vector<const Process*> results;
  // Cleanup
  for (auto it = processes.begin(); it != processes.end();) {
    if (new_pids.find(it->first) == new_pids.end()) {
      it = processes.erase(it);
    } else {
      results.push_back(it->second.get());
      it++;
    }
  }
  // Sort and trim
  sort(results.begin(), results.end(), [](const Process* a, const Process* b) {
    return a->GetPercentCpu() > b->GetPercentCpu();
  });
  results.resize(max_num);
  return results;
}

void Process::Update() {
  proc_taskinfo pti;
  if (sizeof(pti) ==
      proc_pidinfo(GetPid(), PROC_PIDTASKINFO, 0, &pti, sizeof(pti))) {
    if (0 != utime_ || 0 != stime_) {
      unsigned int diff =
          (pti.pti_total_system - stime_) + (pti.pti_total_user - utime_);

      percent_cpu_ =
          1.0 * diff * num_cpus_ /
          ((total_usage_->user_cpu_ticks + total_usage_->system_cpu_ticks +
           total_usage_->idle_cpu_ticks) * 100000.0);
    }

    utime_ = pti.pti_total_system;
    stime_ = pti.pti_total_user;
  }
}

int Process::GetPid() const { return proc_.kp_proc.p_pid; }

string Process::GetCommandName() const {
  auto it = command_.rfind('/');
  if (it == string::npos) {
    return command_;
  }
  return command_.substr(it + 1);
}

string Process::GetCommand() const { return command_; }

double Process::GetPercentCpu() const { return percent_cpu_; }

string Process::GetBase64Icon() const {
  return "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABz"
         "enr0AAABSklEQVRYhWNgGAWDBXz++Hnipw+f/nx8//H/h/cf/"
         "sNoZDaIJkYeA39A0J8+gu2"
         "YgOGA50+f/3n25Nl/auCnj5/CaWQ2kpo/GA64feP2/"
         "zs374AxjA2iicHIesDsm7fhNDIbJg"
         "/CGA5QUNf+T088+B0geEzjv7w5qhh/iRwGpokD5Oy1/"
         "gtcVv8vla75X0GLzg6QytX6z39f4"
         "7/wWo3/Aqc1/ovO06SvA0QWaP4X3qbxX0FDG4zlTOgcAjJ+wOC/"
         "oAHGEnVA3+sMQBqQ19X+"
         "L1mq+V/wvPp/od0adE4DKVr/ZR0hbMkczf98T9T/K2jS0QGiUyCW8t9Q/y9wW/2/"
         "RA2dEyE4CgyA5cBZjf9ydloomujmABAe0IIIjHW0yDacOg6gMR51wKgDRh2AxQE6f+"
         "jnAB3"
         "MVrGihs5E+jhC54+Spg5mv2AUjFgAAH/+wdnTEexFAAAAAElFTkSuQmCC";
}

v8::Local<v8::Object> Process::ToV8Object() const {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> pid_prop = Nan::New("pid").ToLocalChecked();
  v8::Local<v8::String> command_prop = Nan::New("command").ToLocalChecked();
  v8::Local<v8::String> command_name_prop =
      Nan::New("commandName").ToLocalChecked();
  v8::Local<v8::String> icon_prop = Nan::New("icon").ToLocalChecked();
  v8::Local<v8::String> percent_cpu_prop = Nan::New("percentCpu").ToLocalChecked();

  v8::Local<v8::Value> pid_value = Nan::New(GetPid());
  v8::Local<v8::Value> command_value = Nan::New(GetCommand()).ToLocalChecked();
  v8::Local<v8::Value> command_name_value =
      Nan::New(GetCommandName()).ToLocalChecked();
  v8::Local<v8::Value> icon_value = Nan::New(GetBase64Icon()).ToLocalChecked();
  v8::Local<v8::Value> percent_cpu_value = Nan::New(GetPercentCpu());

  Nan::Set(result, pid_prop, pid_value);
  Nan::Set(result, command_prop, command_value);
  Nan::Set(result, command_name_prop, command_name_value);
  Nan::Set(result, icon_prop, icon_value);
  Nan::Set(result, percent_cpu_prop, percent_cpu_value);

  return result;
}

}  // namespace CPUMeter
}  // namespace MacGenius