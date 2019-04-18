import {interval} from 'rxjs';
import {
    processes,
    loadavg,
    totalUsage,
    coresUsage,
} from './genius';

interval(1000).subscribe(() =>{
    console.log(processes().map(x => x.commandName));
});
