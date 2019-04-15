import { interval, Observable, Subject } from 'rxjs';
import { CPUData } from '../../src/interfaces/cpu';
const CPUMeter = require('../../lib/binding/build/Release/darwin_cpu_meter');

export class CPUMonitor {

    public data: Observable<CPUData>;

    private data$ = new Subject<CPUData>();

    constructor() {
        this.data = this.data$;
        interval(3000).subscribe(() => {
            const rawLoadInfo: CPUData = CPUMeter.GetCPUStatistics();
            this.data$.next(rawLoadInfo);
        });
    }
}
