import { TestBed } from '@angular/core/testing';

import { CPUDataService } from './cpu-data.service';

describe('CPUDataService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: CPUDataService = TestBed.get(CPUDataService);
    expect(service).toBeTruthy();
  });
});
