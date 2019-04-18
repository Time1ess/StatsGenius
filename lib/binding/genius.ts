const {
    GetProcesses,
    GetAverageLoad,
    GetCPUTotalUsage,
    GetCPUCoresUsage,
    GetCPUCoresTemperature,
    GetUptime
} = require('./build/Release/genius-bindings');
import { Process } from '../../src/interfaces/process';
import { CPULoad, CPUUsage } from '../../src/interfaces/cpu';

export function processes(n?: number): Process[] {
    n = n || 8;
    const res: Process[] = GetProcesses();
    return res.sort((x, y) => {
        return y.percentCPU - x.percentCPU;
    }).slice(0, n);
}

export function loadavg(): CPULoad {
    const res = GetAverageLoad();
    return {
        loadLast1Minute: res[0],
        loadLast5Minutes: res[1],
        loadLast15Minutes: res[2],
    };
}

export function totalUsage(): CPUUsage {
    const res: number[] = GetCPUTotalUsage();
    return {
        userCPU: res[0] * 100,
        systemCPU: res[1] * 100,
        idleCPU: res[2] * 100,
        niceCPU: res[3] * 100,
    };
}

export function coresUsage(): CPUUsage[] {
    const usages: number[] = GetCPUCoresUsage();
    const cnt = usages.length;  // user, sys, idle, nice, total
    const res: CPUUsage[] = [];
    for (let i = 0; i < cnt; i += 5) {
        res.push({
            userCPU: usages[i] * 100,
            systemCPU: usages[i + 1] * 100,
            idleCPU: usages[i + 2] * 100,
            niceCPU: usages[i + 3] * 100,
        });
    }
    return res;
}

export function temperature(): number {
    const temps: number[] = GetCPUCoresTemperature();
    let sum = 0;
    for (const temp of temps) {
        sum += temp;
    }
    return sum / temps.length;
}

export function uptime(): number {
    return GetUptime();
}
