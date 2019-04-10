import { Injectable, NgZone } from '@angular/core';
import { timer, Subject, Observable } from 'rxjs';

import { CPUUsage, CPUData } from '../../interfaces/cpu';
import { ElectronService } from './electron.service';

@Injectable({
  providedIn: 'root'
})
export class CpuDataService {
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
    if (!this.electronService.isElectron()) {
      timer(0, 1000).subscribe(() => {
        this.cpuData$.next({
          totalUsage: genUsage(),
          coreUsage: genUsages(4),
          processes: [
            {
              processName: 'A',
              processCPU: 23.2,
            },
            {
              processName: 'B',
              processCPU: 15.2,
            },
            {
              processName: 'C',
              processCPU: 12.2,
            },
            {
              processName: 'D',
              processCPU: 8.2,
            },
            {
              processName: 'E',
              processCPU: 5.2,
            },
          ],
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
