import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import {
  MatDividerModule,
  MatCardModule,
  MatIconModule,
  MatListModule,
} from '@angular/material';
import { RouterModule } from '@angular/router';
import { FlexLayoutModule } from '@angular/flex-layout';
import { NgxEchartsModule } from 'ngx-echarts';

import { DurationPipe } from './pipes/duration.pipe';

@NgModule({
  declarations: [
    DurationPipe,
  ],
  imports: [
    CommonModule,
    RouterModule,
    FlexLayoutModule,
    NgxEchartsModule,
    MatCardModule,
    MatDividerModule,
    MatIconModule,
    MatListModule,
  ],
  exports: [
    CommonModule,
    RouterModule,
    FlexLayoutModule,
    NgxEchartsModule,
    MatCardModule,
    MatDividerModule,
    MatIconModule,
    MatListModule,

    DurationPipe,
  ],
})
export class SharedModule { }
