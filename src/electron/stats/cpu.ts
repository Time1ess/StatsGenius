import { EventEmitter } from 'events';
import { interval, Observable, Subject } from 'rxjs';
import { CPUData } from '../../interfaces/cpu';
const CPUMeter = require('../../../lib/binding/build/Release/darwin_cpu_meter');

export class CPUMonitor extends EventEmitter {

    public data: Observable<CPUData>;

    private data$ = new Subject<CPUData>();

    constructor() {
        super();
        this.data = this.data$;
        interval(2000).subscribe(() => {
            const rawLoadInfo: CPUData = CPUMeter.GetCPULoadInfo();
            if (!rawLoadInfo) {
                return;
            }
            this.data$.next(rawLoadInfo);
        });
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
