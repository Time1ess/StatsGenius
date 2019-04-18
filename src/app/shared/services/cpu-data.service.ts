import { Injectable, NgZone } from '@angular/core';
import { timer, Subject, Observable } from 'rxjs';

import { CPUUsage, CPUData, CPULoad } from '../../../interfaces/cpu';
import { Process } from '../../../interfaces/process';
import { ElectronService } from './electron.service';

@Injectable({
  providedIn: 'root'
})
export class CPUDataService {
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

  constructor(
    private readonly electronService: ElectronService,
    private readonly zone: NgZone,
  ) {
    this.totalUsage = this.totalUsage$;
    this.coreUsage = this.coreUsage$;
    this.processes = this.processes$;
    this.averageLoad = this.averageLoad$;
    this.cpuTemperature = this.cpuTemperature$;
    this.uptime = this.uptime$;
    const targets: Array<[Subject<{}>, string]> = [
      [this.totalUsage$, 'total-usage'],
      [this.coreUsage$, 'core-usage'],
      [this.processes$, 'processes'],
      [this.averageLoad$, 'average-load'],
      [this.cpuTemperature$, 'cpu-temperature'],
      [this.uptime$, 'uptime'],
    ];
    for (const target of targets) {
      const [subject, channelName] = target;
      this.electronService.ipcRenderer.on(channelName,
        (event: Electron.IpcMessageEvent, data: {}) => {
          this.zone.run(() => {
            subject.next(data);
          });
        });
    }
  }
}

@Injectable({
  providedIn: 'root'
})
export class CPUTestDataService {
  public cpuData: Observable<CPUData>;

  private cpuData$ = new Subject<CPUData>();

  constructor() {
    this.cpuData = this.cpuData$;
    timer(0, 100).subscribe(() => {
      this.cpuData$.next({
        totalUsage: this.genUsage(),
        coreUsage: this.genUsages(8),
        processes: this.genProcessUsage(8),
        averageLoad: this.genLoad(),
        cpuTemperature: +(Math.random() * 20 + 50).toFixed(1),
        uptime: Math.random() * 30 * 24 * 60 * 60,
      });
    });
  }

  genLoad() {
    return {
      loadLast1Minute: +(Math.random() * 3).toFixed(2),
      loadLast5Minutes: +(Math.random() * 3).toFixed(2),
      loadLast15Minutes: +(Math.random() * 3).toFixed(2),
    } as CPULoad;
  }

  genUsage(): CPUUsage {
    return {
      userCPU: 40 * Math.random(),
      systemCPU: 40 * Math.random(),
      idleCPU: 20 * Math.random(),
      niceCPU: 20 * Math.random(),
    };
  }

  genUsages(total: number): CPUUsage[] {
    const res: CPUUsage[] = [];
    for (let i = 0; i < total; i++) {
      res.push(this.genUsage());
    }
    return res;
  }

  genProcessUsage(total: number): Process[] {
    const res: Process[] = [];
    for (let i = 0; i < total; i++) {
      res.push({
        pid: i,
        commandName: `Process ${i}`,
        command: `${i}`,
        icon: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABz' +
          'enr0AAABSklEQVRYhWNgGAWDBXz++Hnipw+f/nx8//H/h/cf/' +
          'sNoZDaIJkYeA39A0J8+gu2' +
          'YgOGA50+f/3n25Nl/auCnj5/CaWQ2kpo/GA64feP2/' +
          'zs374AxjA2iicHIesDsm7fhNDIbJg' +
          '/CGA5QUNf+T088+B0geEzjv7w5qhh/iRwGpokD5Oy1/' +
          'gtcVv8vla75X0GLzg6QytX6z39f4' +
          '7/wWo3/Aqc1/ovO06SvA0QWaP4X3qbxX0FDG4zlTOgcAjJ+wOC/' +
          'oAHGEnVA3+sMQBqQ19X+' +
          'L1mq+V/wvPp/od0adE4DKVr/ZR0hbMkczf98T9T/K2jS0QGiUyCW8t9Q/y9wW/2/' +
          'RA2dEyE4CgyA5cBZjf9ydloomujmABAe0IIIjHW0yDacOg6gMR51wKgDRh2AxQE6f+' +
          'jnAB3' +
          'MVrGihs5E+jhC54+Spg5mv2AUjFgAAH/+wdnTEexFAAAAAElFTkSuQmCC',
        percentCPU: (total - i) * 10 * Math.random(),
      });
    }
    return res.sort((x, y) => y.percentCPU - x.percentCPU);
  }
}
