import { Injectable, NgZone } from '@angular/core';
import { timer, Subject, Observable } from 'rxjs';

import { CPUUsage, CPUData, ProcessData } from '../../interfaces/cpu';
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
    const genProcessUsage = (total: number): ProcessData[] => {
      const res: ProcessData[] = [];
      for (let i = 0; i < total; i++) {
        res.push({
          processName: `Process ${i}`,
          processCPU: 150 * Math.random(),
        });
      }
      return res.sort((x, y) => y.processCPU - x.processCPU);
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
