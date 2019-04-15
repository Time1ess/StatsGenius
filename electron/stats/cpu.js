"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var rxjs_1 = require("rxjs");
var CPUMeter = require('../../lib/binding/build/Release/darwin_cpu_meter');
var CPUMonitor = /** @class */ (function () {
    function CPUMonitor() {
        var _this = this;
        this.data$ = new rxjs_1.Subject();
        this.data = this.data$;
        rxjs_1.interval(3000).subscribe(function () {
            var rawLoadInfo = CPUMeter.GetCPUStatistics();
            _this.data$.next(rawLoadInfo);
        });
    }
    return CPUMonitor;
}());
exports.CPUMonitor = CPUMonitor;
//# sourceMappingURL=cpu.js.map