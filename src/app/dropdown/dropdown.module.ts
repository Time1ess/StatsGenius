import { NgModule } from '@angular/core';
import { DropdownComponent } from '../dropdown/dropdown.component';


import { CPUDropdownComponent } from './cpu-dropdown/cpu-dropdown.component';
import { SharedModule } from '../shared/shared.module';

@NgModule({
  declarations: [
    DropdownComponent,
    CPUDropdownComponent,
  ],
  imports: [
    SharedModule,
  ]
})
export class DropdownModule { }
