import { Component, OnInit, OnDestroy } from '@angular/core';
import { EChartOption, ECharts } from 'echarts';
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

import { CPUDataService } from '../../shared/services/cpu-data.service';
import { CPUData } from '../../../interfaces/cpu';

@Component({
  selector: 'app-cpu-dropdown',
  templateUrl: './cpu-dropdown.component.html',
  styleUrls: ['./cpu-dropdown.component.scss']
})
export class CPUDropdownComponent implements OnInit, OnDestroy {
  readonly COLOR_BLUE = '#2196f3';  // Blue
  readonly COLOR_RED = '#f44336';  // Red
  readonly COLOR_GRAY = '#9B9B9B';

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

  private historyLabel: string[] = [];
  private historyUserCPU: number[] = [];
  private historySystemCPU: number[] = [];
  private historyLoadLast1Minute: number[] = [];
  private historyLoadLast5Minutes: number[] = [];
  private historyLoadLast15Minutes: number[] = [];
  private peakLoad = 0;

  private readonly destroyed$ = new Subject<void>();
  private cpuUsageChart: ECharts;
  private coreUsageChart: ECharts;
  private averageLoadChart: ECharts;

  constructor(
    readonly cpuDataService: CPUDataService,
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
    this.updateCPUUsageChart();
    this.updateCoreUsageChart();
    this.updateAverageLoadChart();
  }

  private updateData(data: CPUData) {
    this.cpuData = data;
    this.historyLabel.push(new Date().toLocaleTimeString());
    this.historyUserCPU.push(this.cpuData.totalUsage.userCPU);
    this.historySystemCPU.push(this.cpuData.totalUsage.systemCPU);
    this.peakLoad = Math.max(this.peakLoad, this.cpuData.averageLoad.loadLast1Minute);
    this.historyLoadLast1Minute.push(this.cpuData.averageLoad.loadLast1Minute);
    this.historyLoadLast5Minutes.push(this.cpuData.averageLoad.loadLast5Minutes);
    this.historyLoadLast15Minutes.push(this.cpuData.averageLoad.loadLast15Minutes);
    if (this.historyLabel.length > 200) {
      this.historyLabel.shift();
      this.historyUserCPU.shift();
      this.historySystemCPU.shift();
      this.historyLoadLast1Minute.shift();
      this.historyLoadLast5Minutes.shift();
      this.historyLoadLast15Minutes.shift();
    }
  }

  onCPUUsageChartInit(event: ECharts) {
    this.cpuUsageChart = event;
  }

  updateCPUUsageChart() {
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

  onAverageLoadChartInit(event: ECharts) {
    this.averageLoadChart = event;
  }

  updateAverageLoadChart() {
    if (!this.averageLoadChart) {
      return;
    }
    this.averageLoadChart.setOption({
      xAxis: {
        data: this.historyLabel,
      },
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
