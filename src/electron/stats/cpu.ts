import * as child_process from 'child_process';
import { EventEmitter } from 'events';
import { Observable, Subject } from 'rxjs';
import { ipcMain, IpcMessageEvent } from 'electron';


export interface CPUData {
    /** How much CPU is consumed by user processes. */
    userCPU: number;
    /** How much CPU is consumed by system processes. */
    systemCPU: number;
    /** How much CPU is idle. */
    idleCPU: number;
}


export class CPUMonitor extends EventEmitter {
    /** Regular pattern used to extract cpu load status. */
    private static cpuLoadPattern = new RegExp('(\\d*\\.?\\d*)% user, (\\d*\\.?\\d*)% sys, (\\d*\\.?\\d*)% idle');

    public data: Observable<CPUData>;

    private data$ = new Subject<CPUData>();
    private _data: CPUData;
    private topProcess: child_process.ChildProcess;

    constructor() {
        super();
        this.data = this.data$;
        this.topProcess = child_process.spawn(
            '/usr/bin/top',
            ['-stats', 'pid,cpu,command', '-o', 'cpu', '-n', '8', '-s', '2']);

        this.topProcess.stdout.on('data', (data: string) => {
            this.parseTopData(data);
        });

        this.on('exit', () => {
            this.topProcess.kill('SIGINT');
        });

        ipcMain.on('GET:cpu-data', (event: IpcMessageEvent) => {
            event.sender.send('cpu-data', this._data);
        });
    }

    private parseTopData(data: string) {
        const match = CPUMonitor.cpuLoadPattern.exec(data);
        const [userCPU, systemCPU, idleCPU] = match.slice(1, 4).map(x => +x);
        this._data = {userCPU, systemCPU, idleCPU};
        this.data$.next(this._data);
    }
}

if (require.main === module) {
    const monitor = new CPUMonitor();

    monitor.data.subscribe(data => {
        process.stdout.write('User CPU: ' + data.userCPU + '\n');
        process.stdout.write('System CPU: ' + data.systemCPU + '\n');
        process.stdout.write('Idle CPU: ' + data.idleCPU + '\n');
    });
}
