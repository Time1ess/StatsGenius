import { HomeComponent } from './components/home/home.component';
import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { CPUDropdownComponent } from './dropdown/cpu-dropdown/cpu-dropdown.component';
import { DropdownComponent } from './dropdown/dropdown.component';

const routes: Routes = [
    {
        path: 'dropdown',
        component: DropdownComponent,
        children: [
            {
                path: 'cpu',
                component: CPUDropdownComponent,
            },
        ],
    },
    {
        path: '',
        component: HomeComponent
    },
];

@NgModule({
    imports: [RouterModule.forRoot(routes, {
        enableTracing: false,
    })],
    exports: [RouterModule]
})
export class AppRoutingModule { }
