import { Component, OnInit, OnDestroy } from '@angular/core';
import { EChartOption, ECharts } from 'echarts';
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

import { CpuDataService } from '../../services/cpu-data.service';
import { CPUData } from '../../../interfaces/cpu';

@Component({
  selector: 'app-cpu-dropdown',
  templateUrl: './cpu-dropdown.component.html',
  styleUrls: ['./cpu-dropdown.component.scss']
})
export class CPUDropdownComponent implements OnInit, OnDestroy {
  readonly userCPUColor = '#2196f3';  // Blue
  readonly systemCPUColor = '#f44336';  // Red

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
          shadowColor: this.userCPUColor,
          shadowBlur: 1,
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
          shadowColor: this.systemCPUColor,
          shadowBlur: 1,
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
    ).subscribe((data) => this.update(data));
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

  private updateData(data: CPUData) {
    this.cpuData = data;
    this.historyLabel.push(new Date().toLocaleTimeString());
    this.historyUserCPU.push(this.cpuData.totalUsage.userCPU);
    this.historySystemCPU.push(this.cpuData.totalUsage.systemCPU);
    if (this.historyLabel.length > 900) {
      this.historyLabel.shift();
      this.historyUserCPU.shift();
      this.historySystemCPU.shift();
    }
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
