import { Component, OnInit, OnDestroy } from '@angular/core';
import { EChartOption, ECharts } from 'echarts';
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

import { CPUDataService } from '../../shared/services/cpu-data.service';
import { CPUUsage, CPULoad } from '../../../interfaces/cpu';
import { Process } from '../../../interfaces/process';

@Component({
  selector: 'app-cpu-dropdown',
  templateUrl: './cpu-dropdown.component.html',
  styleUrls: ['./cpu-dropdown.component.scss']
})
export class CPUDropdownComponent implements OnInit, OnDestroy {
  readonly COLOR_BLUE = '#2196f3';  // Blue
  readonly COLOR_RED = '#f44336';  // Red
  readonly COLOR_GRAY = '#9B9B9B';

  totalUsageOptions: EChartOption = {
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data: [],
      show: false,
    },
    yAxis: {
      max: 100,
      min: 0,
      show: false,
    },
    series: [
      {
        data: [],
        type: 'line',
        stack: 'usage',
        itemStyle: {
          opacity: 0,
        },
        lineStyle: {
          opacity: 0,
        },
        areaStyle: {
          color: this.COLOR_BLUE,
          shadowColor: this.COLOR_BLUE,
          shadowBlur: 1,
        },
        silent: true,
        animation: false,
        sampling: 'average',
      },
      {
        data: [],
        type: 'line',
        stack: 'usage',
        itemStyle: {
          opacity: 0,
        },
        lineStyle: {
          opacity: 0,
        },
        areaStyle: {
          color: this.COLOR_RED,
          shadowColor: this.COLOR_RED,
          shadowBlur: 1,
        },
        silent: true,
        animation: false,
        sampling: 'average',
      },
    ],
    grid: {
      show: true,
      top: 0,
      bottom: 0,
      left: 0,
      right: 0,
      backgroundColor: '#eeeeee',
    },
  };
  coreUsageOptions: EChartOption = {
    xAxis: {
      type: 'category',
      data: [],
      show: false,
    },
    yAxis: {
      type: 'value',
      max: 100,
      min: 0,
      show: false,
    },
    series: [
      {
        data: [],
        type: 'bar',
        itemStyle: {
          color: this.COLOR_BLUE,
        },
        silent: true,
        animation: false,
        sampling: 'average',
      },
      {
        data: [],
        type: 'bar',
        itemStyle: {
          color: this.COLOR_RED,
        },
        silent: true,
        animation: false,
        sampling: 'average',
      },
    ],
    grid: {
      show: true,
      top: 0,
      bottom: 0,
      left: 0,
      right: 0,
      backgroundColor: '#eeeeee',
    },
  };
  averageLoadOptions: EChartOption = {
    title: {
      text: '',
      textStyle: {
        fontSize: 13,
        fontWeight: 400,
      },
      top: 'bottom',
      left: 'center',
    },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      data: [],
      show: false,
    },
    yAxis: {
      max: 5,
      min: 0,
      show: false,
    },
    series: [
      {
        data: [],
        type: 'line',
        itemStyle: {
          opacity: 0,
        },
        lineStyle: {
          opacity: 0,
        },
        areaStyle: {
          color: this.COLOR_BLUE,
          shadowColor: this.COLOR_BLUE,
          shadowBlur: 1,
        },
        silent: true,
        animation: false,
        sampling: 'average',
      },
      {
        data: [],
        type: 'line',
        itemStyle: {
          opacity: 0,
        },
        lineStyle: {
          color: this.COLOR_RED,
        },
        silent: true,
        animation: false,
        sampling: 'average',
      },
      {
        data: [],
        type: 'line',
        itemStyle: {
          opacity: 0,
        },
        lineStyle: {
          color: this.COLOR_GRAY,
        },
        silent: true,
        animation: false,
        sampling: 'average',
      },
    ],
    grid: {
      show: true,
      top: 0,
      bottom: 30,
      left: 0,
      right: 0,
      backgroundColor: '#eeeeee',
    },
  };

  totalUsage: CPUUsage = null;
  cpuTemperature: number = null;
  processes: Process[] = [];
  averageLoad: CPULoad = null;
  uptime: number = null;

  private historyUserCPU: number[] = [];
  private historySystemCPU: number[] = [];
  private historyLoadLast1Minute: number[] = [];
  private historyLoadLast5Minutes: number[] = [];
  private historyLoadLast15Minutes: number[] = [];
  private peakLoad = 0;

  private readonly destroyed$ = new Subject<void>();
  private totalUsageChart: ECharts;
  private coreUsageChart: ECharts;
  private averageLoadChart: ECharts;

  constructor(
    readonly cpuDataService: CPUDataService,
  ) { }

  ngOnInit() {
    this.cpuDataService.totalUsage.pipe(
      takeUntil(this.destroyed$),
    ).subscribe((data) => this.updateTotalUsage(data));
    this.cpuDataService.coreUsage.pipe(
      takeUntil(this.destroyed$),
    ).subscribe((data) => this.updateCoreUsage(data));
    this.cpuDataService.processes.pipe(
      takeUntil(this.destroyed$),
    ).subscribe((data) => this.processes = data);
    this.cpuDataService.averageLoad.pipe(
      takeUntil(this.destroyed$),
    ).subscribe((data) => this.updateAverageLoad(data));
    this.cpuDataService.cpuTemperature.pipe(
      takeUntil(this.destroyed$),
    ).subscribe((data) => this.cpuTemperature = data);
    this.cpuDataService.uptime.pipe(
      takeUntil(this.destroyed$),
    ).subscribe((data) => this.uptime = data);
  }

  ngOnDestroy() {
    // Called once, before the instance is destroyed.
    this.destroyed$.next();
  }

  private trimData(data: Array<{}>) {
    if (data.length > 200) {
      data.shift();
    }
  }

  onTotalUsageChartInit(event: ECharts) {
    this.totalUsageChart = event;
  }

  updateTotalUsage(data: CPUUsage) {
    this.totalUsage = data;
    this.historyUserCPU.push(data.userCPU);
    this.historySystemCPU.push(data.systemCPU);
    this.trimData(this.historyUserCPU);
    this.trimData(this.historySystemCPU);
    if (!this.totalUsageChart) {
      return;
    }
    this.totalUsageChart.setOption({
      series: [
        {
          data: this.historyUserCPU,
        },
        {
          data: this.historySystemCPU,
        }
      ],
    });
  }

  onCoreUsageChartInit(event: ECharts) {
    this.coreUsageChart = event;
  }

  updateCoreUsage(data: CPUUsage[]) {
    if (!this.coreUsageChart) {
      return;
    }
    this.coreUsageChart.setOption({
      series: [
        {
          data: data.map(x => x.userCPU),
          stack: 'usage',
          type: 'bar',
        },
        {
          data: data.map(x => x.systemCPU),
          stack: 'usage',
          type: 'bar',
        },
      ],
    });
  }

  onAverageLoadChartInit(event: ECharts) {
    this.averageLoadChart = event;
  }

  updateAverageLoad(data: CPULoad) {
    this.averageLoad = data;
    this.peakLoad = Math.max(this.peakLoad, data.loadLast1Minute);
    this.historyLoadLast1Minute.push(data.loadLast1Minute);
    this.historyLoadLast5Minutes.push(data.loadLast5Minutes);
    this.historyLoadLast15Minutes.push(data.loadLast15Minutes);
    this.trimData(this.historyLoadLast1Minute);
    this.trimData(this.historyLoadLast5Minutes);
    this.trimData(this.historyLoadLast15Minutes);
    if (!this.averageLoadChart) {
      return;
    }
    this.averageLoadChart.setOption({
      title: {
        text: `Peak Load: ${this.peakLoad.toFixed(2)}`,
      },
      series: [
        {
          data: this.historyLoadLast1Minute,
        },
        {
          data: this.historyLoadLast5Minutes,
        },
        {
          data: this.historyLoadLast15Minutes,
        }
      ],
    });
  }
}
