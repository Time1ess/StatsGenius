import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { DropdownComponent } from '../dropdown/dropdown.component';
import { RouterModule } from '@angular/router';
import {
  MatDividerModule,
  MatCardModule,
  MatListModule,
} from '@angular/material';

import { FlexLayoutModule } from '@angular/flex-layout';
import { NgxEchartsModule } from 'ngx-echarts';

import { CPUDropdownComponent } from './cpu-dropdown/cpu-dropdown.component';

@NgModule({
  declarations: [
    DropdownComponent,
    CPUDropdownComponent,
  ],
  imports: [
    CommonModule,
    RouterModule,
    FlexLayoutModule,
    NgxEchartsModule,
    MatCardModule,
    MatDividerModule,
    MatListModule,
  ]
})
export class DropdownModule { }
