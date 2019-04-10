import * as child_process from 'child_process';
import { EventEmitter } from 'events';
import { Observable, Subject } from 'rxjs';
import { CPUData } from '../../interfaces/cpu';

export class CPUMonitor extends EventEmitter {
    /** Regular pattern used to extract cpu load status. */
    private static cpuUsagePattern = new RegExp('(\\d*\\.?\\d*)% user, (\\d*\\.?\\d*)% sys, (\\d*\\.?\\d*)% idle');
    private static cpuLoadPattern = new RegExp('vm.loadavg: { (\\d*\\.?\\d*) (\\d*\\.?\\d*) (\\d*\\.?\\d*) }');

    public data: Observable<CPUData>;

    private data$ = new Subject<CPUData>();
    private topProcess: child_process.ChildProcess;

    constructor() {
        super();
        this.data = this.data$;
        this.topProcess = child_process.spawn('/usr/bin/top', ['-stats', 'pid,cpu,command', '-o', 'cpu', '-n', '8', '-s', '1']);
        this.topProcess.stdout.on('data', (data: string) => {
            this.data$.next(this.parseData(data));
        });
    }

    private parseData(data: string): CPUData {
        const match = CPUMonitor.cpuUsagePattern.exec(data);
        const [userCPU, systemCPU, idleCPU] = match.slice(1, 4).map(x => +x);
        return {
            totalUsage: { userCPU, systemCPU, idleCPU },
            coreUsage: [],
            processes: [],
        };
    }
}

if (require.main === module) {
    const monitor = new CPUMonitor();

    monitor.data.subscribe(data => {
        process.stdout.write('User CPU: ' + data.totalUsage.userCPU + '\n');
        process.stdout.write('System CPU: ' + data.totalUsage.systemCPU + '\n');
        process.stdout.write('Idle CPU: ' + data.totalUsage.idleCPU + '\n');
    });
}