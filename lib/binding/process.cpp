#include <nan.h>
#include <iostream>

#include "process.hpp"
#ifdef __APPLE__
#include "darwin/process.hpp"
using namespace MacGenius::Darwin;
#elif _WIN32
#elif __linux__
#else
#error No operating system defined
#endif


namespace MacGenius {
v8::Local<v8::Object> ProcessToV8Object(Process& p) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> pid_prop = Nan::New("pid").ToLocalChecked();
  v8::Local<v8::String> command_prop = Nan::New("command").ToLocalChecked();
  v8::Local<v8::String> command_name_prop =
      Nan::New("commandName").ToLocalChecked();
  v8::Local<v8::String> icon_prop = Nan::New("icon").ToLocalChecked();
  v8::Local<v8::String> percent_cpu_prop =
      Nan::New("percentCPU").ToLocalChecked();

  v8::Local<v8::Value> pid_value = Nan::New(p.pid);
  v8::Local<v8::Value> command_value = Nan::New(p.command).ToLocalChecked();
  v8::Local<v8::Value> command_name_value =
      Nan::New(p.command_name).ToLocalChecked();
  v8::Local<v8::Value> icon_value = Nan::New(p.icon).ToLocalChecked();
  v8::Local<v8::Value> percent_cpu_value = Nan::New(p.percent_cpu);

  Nan::Set(result, pid_prop, pid_value);
  Nan::Set(result, command_prop, command_value);
  Nan::Set(result, command_name_prop, command_name_value);
  Nan::Set(result, icon_prop, icon_value);
  Nan::Set(result, percent_cpu_prop, percent_cpu_value);

  return result;
}

NAN_METHOD(ExportedGetProcesses) {
  vector<Process> processes;
  processes = GetProcesses();
  v8::Local<v8::Array> results = Nan::New<v8::Array>();
  for (size_t i = 0; i < processes.size(); i++) {
    v8::Local<v8::Object> process_value = ProcessToV8Object(processes[i]);
    Nan::Set(results, i, process_value);
  }
  info.GetReturnValue().Set(results);
}

void ProcessInit(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("GetProcesses").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(ExportedGetProcesses)->GetFunction());
}

NODE_MODULE(Process, ProcessInit)
}  // namespace MacGenius