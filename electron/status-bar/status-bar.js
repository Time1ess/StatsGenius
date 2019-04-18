"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var electron_1 = require("electron");
var path = require("path");
var url = require("url");
var cpu_1 = require("../stats/cpu");
var isDev = require('electron-is-dev');
var StatusBarItem = /** @class */ (function () {
    function StatusBarItem() {
        var _this = this;
        this.monitor = new cpu_1.CPUMonitor();
        this.createWindow();
        this.createTray();
        this.monitor.totalUsage.subscribe(function (data) {
            _this.window.webContents.send('total-usage', data);
            var cpu = Math.round(data.userCPU + data.systemCPU);
            if (isNaN(cpu)) {
                cpu = 0;
            }
            _this.tray.setTitle("CPU: " + cpu + "%");
        });
        var subscriptionTargets = [
            [this.monitor.coreUsage, 'core-usage'],
            [this.monitor.processes, 'processes'],
            [this.monitor.averageLoad, 'average-load'],
            [this.monitor.cpuTemperature, 'cpu-temperature'],
            [this.monitor.uptime, 'uptime'],
        ];
        var _loop_1 = function (item) {
            var obs = item[0], channelName = item[1];
            obs.subscribe(function (data) {
                _this.window.webContents.send(channelName, data);
            });
        };
        for (var _i = 0, subscriptionTargets_1 = subscriptionTargets; _i < subscriptionTargets_1.length; _i++) {
            var item = subscriptionTargets_1[_i];
            _loop_1(item);
        }
    }
    StatusBarItem.prototype.createTray = function () {
        this.tray = new electron_1.Tray(path.join(__dirname, '..', 'assets', 'images', 'magic-lamp_16x16.png'));
        this.tray.setToolTip('MacGenius');
        this.tray.on('click', this.onClick.bind(this));
    };
    StatusBarItem.prototype.createWindow = function () {
        var _this = this;
        this.window = new electron_1.BrowserWindow({
            resizable: false,
            show: false,
            frame: false,
            width: 280,
        });
        this.window.on('blur', function () {
            _this.window.hide();
        });
        this.window.setVisibleOnAllWorkspaces(true);
        if (isDev) {
            this.window.webContents.openDevTools();
            this.window.loadURL('http://localhost:4200/');
        }
        else {
            this.window.loadURL(url.format({
                pathname: path.join(__dirname, '../../dist/index.html'),
                protocol: 'file:',
            }));
        }
        this.window.webContents.once('dom-ready', function () {
            _this.window.webContents.send('navigate-to-route', '/dropdown/cpu');
        });
        return this.window;
    };
    StatusBarItem.prototype.hideWindow = function () {
        this.window.hide();
    };
    StatusBarItem.prototype.showWindow = function (bounds) {
        this.window.setPosition(bounds.x, bounds.y);
        this.window.show();
    };
    StatusBarItem.prototype.onClick = function (e, bounds) {
        if (this.window.isVisible()) {
            return this.hideWindow();
        }
        this.showWindow(bounds);
    };
    return StatusBarItem;
}());
var StatusBar = /** @class */ (function () {
    function StatusBar() {
        this.items = [];
        var item = new StatusBarItem();
        this.items.push(item);
    }
    return StatusBar;
}());
exports.StatusBar = StatusBar;
//# sourceMappingURL=status-bar.js.map