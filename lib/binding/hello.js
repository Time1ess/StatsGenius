var addon = require('./build/Release/darwin_cpu_meter');

const cnt = 10;
const start = Date.now();
for (let i = 0; i < cnt; i++) {
    const res = addon.GetCPUStatistics();
    console.log(res.cpuTemperature);
}
const end = Date.now();
console.log(`${start} -> ${end}, elapsed: ${end - start} ms, calls per second: ${1000 * cnt / (end - start)}, per call: ${(end - start) / cnt } ms`);
