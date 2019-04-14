import { Injectable, NgZone } from '@angular/core';
import { timer, Subject, Observable } from 'rxjs';

import { CPUUsage, CPUData, ProcessData } from '../../interfaces/cpu';
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
    const genUsage = (): CPUUsage => {
      return {
            userCPU: 40 * Math.random(),
            systemCPU: 40 * Math.random(),
            idleCPU: 20 * Math.random(),
      };
    };
    const genUsages = (total: number): CPUUsage[] => {
      const res: CPUUsage[] = [];
      for (let i = 0; i < total; i++) {
        res.push(genUsage());
      }
      return res;
    };
    const genProcessUsage = (total: number): ProcessData[] => {
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
    };
    if (!this.electronService.isElectron()) {
      timer(0, 1000).subscribe(() => {
        this.cpuData$.next({
          totalUsage: genUsage(),
          coreUsage: genUsages(8),
          processes: genProcessUsage(8),
        });
      });
      return;
    }
    this.electronService.ipcRenderer.on('cpu-data',
      (event: Electron.IpcMessageEvent, data: CPUData) => {
        this.zone.run(() => {
          this.cpuData$.next(data);
        });
      });
  }
}
