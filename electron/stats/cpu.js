"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var events_1 = require("events");
var rxjs_1 = require("rxjs");
var CPUMeter = require('../../lib/binding/build/Release/darwin_cpu_meter');
var CPUMonitor = /** @class */ (function (_super) {
    __extends(CPUMonitor, _super);
    function CPUMonitor() {
        var _this = _super.call(this) || this;
        _this.data$ = new rxjs_1.Subject();
        _this.data = _this.data$;
        rxjs_1.interval(2000).subscribe(function () {
            var rawLoadInfo = CPUMeter.GetCPUStatistics();
            if (!rawLoadInfo) {
                return;
            }
            _this.data$.next(rawLoadInfo);
        });
        return _this;
    }
    return CPUMonitor;
}(events_1.EventEmitter));
exports.CPUMonitor = CPUMonitor;
if (require.main === module) {
    var monitor = new CPUMonitor();
    monitor.data.subscribe(function (data) {
        process.stdout.write('User CPU: ' + data.totalUsage.userCPU + '\n');
        process.stdout.write('System CPU: ' + data.totalUsage.systemCPU + '\n');
        process.stdout.write('Idle CPU: ' + data.totalUsage.idleCPU + '\n');
    });
}
//# sourceMappingURL=cpu.js.map