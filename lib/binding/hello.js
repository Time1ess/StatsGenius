var addon = require('./build/Release/mac-genius-bindings');

const cnt = 100;
const start = Date.now();
for (let i = 0; i < cnt; i++) {
    const processes = addon.GetProcesses();
}
const end = Date.now();
console.log(`${start} -> ${end}, elapsed: ${end - start} ms, calls per second: ${1000 * cnt / (end - start)}, per call: ${(end - start) / cnt } ms`);
