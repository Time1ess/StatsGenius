#include "cpu_load.hpp"

namespace StatsGenius {
namespace CPUMeter {

CPULoad::CPULoad(loadavg average_load) : average_load_{average_load} {}

v8::Local<v8::Object> CPULoad::ToV8Object() {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  v8::Local<v8::String> load_1m_prop =
      Nan::New("loadLast1Minute").ToLocalChecked();
  v8::Local<v8::String> load_5m_prop =
      Nan::New("loadLast5Minutes").ToLocalChecked();
  v8::Local<v8::String> load_15m_prop =
      Nan::New("loadLast15Minutes").ToLocalChecked();

  v8::Local<v8::Value> load_1m_value =
      Nan::New(1.0 * average_load_.ldavg[0] / average_load_.fscale);
  v8::Local<v8::Value> load_5m_value =
      Nan::New(1.0 * average_load_.ldavg[1] / average_load_.fscale);

  v8::Local<v8::Value> load_15m_value =
      Nan::New(1.0 * average_load_.ldavg[2] / average_load_.fscale);

  Nan::Set(result, load_1m_prop, load_1m_value);
  Nan::Set(result, load_5m_prop, load_5m_value);
  Nan::Set(result, load_15m_prop, load_15m_value);

  return result;
}

}  // namespace CPUMeter
}  // namespace StatsGenius