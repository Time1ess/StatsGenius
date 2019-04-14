import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'duration'
})
export class DurationPipe implements PipeTransform {
  private readonly defaultScales = ['week', 'day', 'hour', 'minute'];
  // Scale in seconds
  private readonly convertions = [7 * 24 * 60 * 60,  24 * 60 * 60, 60 * 60, 60];

  transform(val?: number, scales?: string[]): string {
    if (!val) {
      return 'Unknown';
    }
    if (!scales || scales.length !== this.convertions.length) {
      scales = this.defaultScales;
    }
    let res = '';
    for (let i = 0; i < this.convertions.length; i++) {
      const scale = scales[i];
      const cnt = Math.floor(val / this.convertions[i]);
      val = val % this.convertions[i];
      if (cnt === 0) {
        continue;
      } else if (cnt === 1) {
        res += `, 1 ${scale}`;
      } else {
        res += `, ${cnt} ${scale}s`;
      }
    }
    return res.length === 0 ? 'Unknown' : res.substr(2);
  }
}
