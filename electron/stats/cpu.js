"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var rxjs_1 = require("rxjs");
var genius = require("../../lib/binding/genius");
var CPUMonitor = /** @class */ (function () {
    function CPUMonitor() {
        var _this = this;
        this.totalUsage$ = new rxjs_1.Subject();
        this.coreUsage$ = new rxjs_1.Subject();
        this.processes$ = new rxjs_1.Subject();
        this.averageLoad$ = new rxjs_1.Subject();
        this.cpuTemperature$ = new rxjs_1.Subject();
        this.uptime$ = new rxjs_1.Subject();
        this.totalUsage = this.totalUsage$;
        this.coreUsage = this.coreUsage$;
        this.processes = this.processes$;
        this.averageLoad = this.averageLoad$;
        this.cpuTemperature = this.cpuTemperature$;
        this.uptime = this.uptime$;
        rxjs_1.timer(0, 3 * 1000).subscribe(function () {
            _this.totalUsage$.next(genius.totalUsage());
            _this.coreUsage$.next(genius.coresUsage());
            _this.averageLoad$.next(genius.loadavg());
            _this.processes$.next(genius.processes());
        });
        rxjs_1.timer(0, 6 * 1000).subscribe(function () {
            _this.cpuTemperature$.next(genius.temperature());
        });
        rxjs_1.timer(0, 60 * 1000).subscribe(function () {
            _this.uptime$.next(genius.uptime());
        });
    }
    return CPUMonitor;
}());
exports.CPUMonitor = CPUMonitor;
//# sourceMappingURL=cpu.js.map