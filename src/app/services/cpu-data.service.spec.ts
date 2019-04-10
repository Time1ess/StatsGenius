import { TestBed } from '@angular/core/testing';

import { CpuDataService } from './cpu-data.service';

describe('CpuDataService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: CpuDataService = TestBed.get(CpuDataService);
    expect(service).toBeTruthy();
  });
});
