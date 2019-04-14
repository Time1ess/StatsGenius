export interface CPUUsage {
    /** How much CPU is consumed by user processes. */
    userCPU: number;
    /** How much CPU is consumed by system processes. */
    systemCPU: number;
    /** How much CPU is idle. */
    idleCPU: number;
}

export interface ProcessData {
    pid: number;
    commandName: string;
    command: string;
    icon: string;
    percentCPU: number;
}
export interface CPUData {
    totalUsage?: CPUUsage;
    /** How much CPU is used w.r.t to CPU cores. */
    coreUsage?: CPUUsage[];
    processes?: ProcessData[];
}
