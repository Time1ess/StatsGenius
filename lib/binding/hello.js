var addon = require('./build/Release/darwin_cpu_meter');
var rxjs = require('rxjs');

rxjs.interval(2000).subscribe(() => {
    const loadInfo = addon.GetCPULoadInfo();
    console.log(
        100 * loadInfo.totalUsage.userCPU / loadInfo.totalUsage.totalCPU,
        100 * loadInfo.totalUsage.systemCPU / loadInfo.totalUsage.totalCPU,
        100 * loadInfo.totalUsage.idleCPU / loadInfo.totalUsage.totalCPU,
    );
});