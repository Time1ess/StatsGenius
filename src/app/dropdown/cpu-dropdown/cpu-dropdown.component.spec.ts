import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { CPUDropdownComponent } from './cpu-dropdown.component';

describe('CPUDropdownComponent', () => {
  let component: CPUDropdownComponent;
  let fixture: ComponentFixture<CPUDropdownComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ CPUDropdownComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(CPUDropdownComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
