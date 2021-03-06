import { Process } from "./process";

export interface CPUUsage {
    /** How much CPU is consumed by user processes. */
    userCPU: number;
    /** How much CPU is consumed by system processes. */
    systemCPU: number;
    /** How much CPU is idle. */
    idleCPU: number;
    /** How much CPU is cconsumed by niced processes. */
    niceCPU: number;
}

export interface CPULoad {
    loadLast1Minute: number;
    loadLast5Minutes: number;
    loadLast15Minutes: number;
}

export interface CPUData {
    totalUsage?: CPUUsage;
    /** How much CPU is used w.r.t to CPU cores. */
    coreUsage?: CPUUsage[];
    processes?: Process[];
    averageLoad?: CPULoad;
    cpuTemperature?: number;
    uptime?: number;
}
