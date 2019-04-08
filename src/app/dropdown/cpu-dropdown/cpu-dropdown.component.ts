import { Component, OnInit, OnDestroy } from '@angular/core';
import { ElectronService } from '../../providers/electron.service';
import { Subject, timer } from 'rxjs';
import { takeUntil } from 'rxjs/operators';
import { CPUData } from '../../../electron/stats/cpu';
import { EChartOption, graphic } from 'echarts';

@Component({
  selector: 'app-cpu-dropdown',
  templateUrl: './cpu-dropdown.component.html',
  styleUrls: ['./cpu-dropdown.component.scss']
})
export class CPUDropdownComponent implements OnInit, OnDestroy {
  chartOption: EChartOption = {
    xAxis: {
      type: 'category',
      data: ['1月', '2月', '3月', '4月', '5月', '6月', '7月', '8月', '9月', '10月', '11月', '12月'],
      silent: false,
      splitLine: {
        show: false
      },
    },
    tooltip: {},
    yAxis: {},
    series: [{
      data: [820, 932, 901, 934, 1290, 1330, 1320, 1333, 1340, 1568, 1622, 1588],
      type: 'line',
      smooth: true,
      symbol: 'none',
      sampling: 'average',
      itemStyle: {
        normal: {
          color: 'rgb(47, 180, 250)',
        },
      },
      areaStyle: {
        color: new graphic.LinearGradient(0, 0, 0, 1, [{
          offset: 0,
          color: 'rgb(255, 158, 68)'
        }, {
          offset: 1,
          color: 'rgb(255, 70, 131)'
        }])
      },
    }],
    animationEasing: 'elasticOut',
    animationDelayUpdate: /* istanbul ignore next */ (idx: number) => {
      return idx * 50;
    },
  };

  private readonly destroyed$ = new Subject<void>();
  private _cpuData: CPUData = { userCPU: 10, systemCPU: 20, idleCPU: 70 };

  constructor(
    private readonly electronService: ElectronService,
  ) { }

  get cpuData() {
    return this._cpuData;
  }

  ngOnInit() {
    if (this.electronService.isElectron()) {
      this.electronService.ipcRenderer.on('cpu-data', (event: Electron.IpcMessageEvent, data: CPUData) => {
        this._cpuData = data;
      });
      timer(0, 2000).pipe(takeUntil(this.destroyed$)).subscribe(() => {
        this.electronService.ipcRenderer.send('GET:cpu-data');
      });
    }
  }

  ngOnDestroy() {
    this.destroyed$.next();
  }

}
