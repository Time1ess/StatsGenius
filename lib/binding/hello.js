var addon = require('./build/Release/darwin_cpu_meter');

const cnt = 20;
const start = Date.now();
for (let i = 0; i < cnt; i++) {
    const res = addon.GetCPUStatistics();
    console.log(res.averageLoad);
}
const end = Date.now();
console.log(`${start} -> ${end}, Elapsed: ${end - start} ms, Calls per second: ${1000 * cnt / (end - start)}`);
