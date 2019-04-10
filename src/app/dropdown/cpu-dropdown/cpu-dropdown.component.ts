import { Component, OnInit, OnDestroy } from '@angular/core';
import { EChartOption, graphic, ECharts, EChartsSeriesType } from 'echarts';
import { Subject } from 'rxjs';
import { takeUntil, throttleTime, tap } from 'rxjs/operators';

import { CpuDataService } from '../../services/cpu-data.service';
import { CPUData } from '../../../interfaces/cpu';

@Component({
  selector: 'app-cpu-dropdown',
  templateUrl: './cpu-dropdown.component.html',
  styleUrls: ['./cpu-dropdown.component.scss']
})
export class CPUDropdownComponent implements OnInit, OnDestroy {
  readonly userCPUColor = '#f44336';
  readonly systemCPUColor = '#2196f3';
  /* How ofthen should be update the chart. (in seconds) */
  readonly updateInterval = 2;
  /* How much of data recently received should be displayed. (in seconds) */
  readonly recentlyDataLimit = 30 * 60;

  cpuData: CPUData = null;
  cpuUsageOptions: EChartOption = {
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
          color: this.userCPUColor,
        },
        silent: true,
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
          color: this.systemCPUColor,
        },
        silent: true,
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
          color: this.userCPUColor,
        },
        silent: true,
      },
      {
        data: [],
        type: 'bar',
        itemStyle: {
          color: this.systemCPUColor,
        },
        silent: true,
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

  private historyLabel: string[] = [];
  private historyUserCPU: number[] = [];
  private historySystemCPU: number[] = [];
  private readonly destroyed$ = new Subject<void>();
  private cpuUsageChart: ECharts;
  private coreUsageChart: ECharts;

  constructor(
    readonly cpuDataService: CpuDataService,
  ) { }

  ngOnInit() {
    this.cpuDataService.cpuData.pipe(
      takeUntil(this.destroyed$),
      throttleTime(this.updateInterval * 1000),
    ).subscribe((data) => this.update(data));
  }

  private updateData(data: CPUData) {
    this.cpuData = data;
    this.historyLabel.push(new Date().toLocaleTimeString());
    this.historyUserCPU.push(this.cpuData.totalUsage.userCPU);
    this.historySystemCPU.push(this.cpuData.totalUsage.systemCPU);
    if (this.historyLabel.length > this.recentlyDataLimit / this.updateInterval) {
      this.historyLabel.shift();
      this.historyUserCPU.shift();
      this.historySystemCPU.shift();
    }
  }

  ngOnDestroy() {
    // Called once, before the instance is destroyed.
    this.destroyed$.next();
  }

  update(data: CPUData) {
    this.updateData(data);
    this.updateCpuUsageChart();
    this.updateCoreUsageChart();
  }

  onCpuUsageChartInit(event: ECharts) {
    this.cpuUsageChart = event;
  }

  updateCpuUsageChart() {
    if (!this.cpuUsageChart) {
      return;
    }
    this.cpuUsageChart.setOption({
      xAxis: {
        data: this.historyLabel,
      },
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

  updateCoreUsageChart() {
    if (!this.coreUsageChart) {
      return;
    }
    this.coreUsageChart.setOption({
      series: [
        {
          data: this.cpuData.coreUsage.map(x => x.userCPU),
          stack: 'usage',
          type: 'bar',
        },
        {
          data: this.cpuData.coreUsage.map(x => x.systemCPU),
          stack: 'usage',
          type: 'bar',
        },
      ],
    });
  }

}
