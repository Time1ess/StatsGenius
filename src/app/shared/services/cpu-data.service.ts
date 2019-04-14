import { Injectable, NgZone } from '@angular/core';
import { timer, Subject, Observable } from 'rxjs';

import { CPUUsage, CPUData, ProcessData, CPULoad } from '../../../interfaces/cpu';
import { ElectronService } from './electron.service';

@Injectable({
  providedIn: 'root'
})
export class CPUDataService {
  public cpuData: Observable<CPUData>;

  private cpuData$ = new Subject<CPUData>();

  constructor(
    private readonly electronService: ElectronService,
    private readonly zone: NgZone,
  ) {
    this.cpuData = this.cpuData$;
    this.electronService.ipcRenderer.on('cpu-data',
      (event: Electron.IpcMessageEvent, data: CPUData) => {
        this.zone.run(() => {
          this.cpuData$.next(data);
        });
      });
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
    timer(0, 2000).subscribe(() => {
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
    };
  }

  genUsages(total: number): CPUUsage[] {
    const res: CPUUsage[] = [];
    for (let i = 0; i < total; i++) {
      res.push(this.genUsage());
    }
    return res;
  }

  genProcessUsage(total: number): ProcessData[] {
    const res: ProcessData[] = [];
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
