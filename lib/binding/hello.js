var addon = require('./build/Release/darwin_cpu_meter');
var rxjs = require('rxjs');

rxjs.interval(2000).subscribe(() => {
    const loadInfo = addon.GetCPULoadInfo();
    console.log(
        loadInfo.totalUsage.userCPU,
        loadInfo.totalUsage.systemCPU,
        loadInfo.totalUsage.idleCPU,
    );
});