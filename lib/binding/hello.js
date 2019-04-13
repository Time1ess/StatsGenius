var addon = require('./build/Release/darwin_cpu_meter');

let cnt = 10;
while (cnt < 1000000) {
    const start = Date.now();
    for (let i = 0; i < cnt; i++) {
        const res = addon.GetCPUStatistics();
    }
    const end = Date.now();
    console.log(`${start} -> ${end}, Elapsed: ${end - start} ms, Calls per second: ${1000 * cnt / (end - start)}`);
    cnt *= Math.random() * 10;
}
