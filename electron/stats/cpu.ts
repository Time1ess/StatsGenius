import { timer, Observable, Subject } from 'rxjs';
import { CPUUsage, CPULoad } from '../../src/interfaces/cpu';
import * as genius from '../../lib/binding/genius';
import { Process } from '../../src/interfaces/process';

export class CPUMonitor {

    public totalUsage: Observable<CPUUsage>;
    public coreUsage: Observable<CPUUsage[]>;
    public processes: Observable<Process[]>;
    public averageLoad: Observable<CPULoad>;
    public cpuTemperature: Observable<number>;
    public uptime: Observable<number>;

    private totalUsage$ = new Subject<CPUUsage>();
    private coreUsage$ = new Subject<CPUUsage[]>();
    private processes$ = new Subject<Process[]>();
    private averageLoad$ = new Subject<CPULoad>();
    private cpuTemperature$ = new Subject<number>();
    private uptime$ = new Subject<number>();

    constructor() {
        this.totalUsage = this.totalUsage$;
        this.coreUsage = this.coreUsage$;
        this.processes = this.processes$;
        this.averageLoad = this.averageLoad$;
        this.cpuTemperature = this.cpuTemperature$;
        this.uptime = this.uptime$;
        timer(0, 5 * 1000).subscribe(() => {
            this.totalUsage$.next(genius.totalUsage());
            this.coreUsage$.next(genius.coresUsage());
            this.averageLoad$.next(genius.loadavg());
            this.processes$.next(genius.processes());
        });
        timer(0, 10 * 1000).subscribe(() => {
            this.cpuTemperature$.next(genius.temperature());
        });
        timer(0, 60 * 1000).subscribe(() => {
            this.uptime$.next(genius.uptime());
        });
    }
}
