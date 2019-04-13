#pragma once

#include <nan.h>
#include <sys/sysctl.h>

namespace MacGenius {
namespace CPUMeter {

class CPULoad {
 public:
  CPULoad(loadavg);
  v8::Local<v8::Object> ToV8Object();

 private:
   loadavg average_load_;
};

}  // namespace CPUMeter
}  // namespace MacGenius