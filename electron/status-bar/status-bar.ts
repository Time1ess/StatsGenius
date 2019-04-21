import { Tray, BrowserWindow, Rectangle } from 'electron';
import * as path from 'path';
import * as url from 'url';

import { CPUMonitor } from '../stats/cpu';
import { Observable } from 'rxjs';

const isDev = require('electron-is-dev');


class StatusBarItem {
    private tray: Tray;
    private window: BrowserWindow;
    private monitor = new CPUMonitor();

    constructor() {
        this.createWindow();
        this.createTray();
        this.monitor.totalUsage.subscribe(data => {
            this.window.webContents.send('total-usage', data);
            let cpu = Math.round(data.userCPU + data.systemCPU);
            if (isNaN(cpu)) {
                cpu = 0;
            }
            this.tray.setTitle(`CPU: ${cpu}%`);
        });
        const subscriptionTargets: Array<[Observable<{}>, string]> = [
            [this.monitor.coreUsage, 'core-usage'],
            [this.monitor.processes, 'processes'],
            [this.monitor.averageLoad, 'average-load'],
            [this.monitor.cpuTemperature, 'cpu-temperature'],
            [this.monitor.uptime, 'uptime'],
        ];
        for (const item of subscriptionTargets) {
            const [obs, channelName] = item;
            obs.subscribe(data => {
                this.window.webContents.send(channelName, data);
            });
        }
    }

    private createTray() {
        this.tray = new Tray(path.join(__dirname, '..', 'assets', 'images', 'magic-lamp_16x16.png'));
        this.tray.setToolTip('StatsGenius');
        this.tray.on('click', this.onClick.bind(this));
    }

    private createWindow() {
        this.window = new BrowserWindow({
            resizable: false,
            show: false,
            frame: false,
            width: 280,
        });
        this.window.on('blur', () => {
            this.window.hide();
        });
        this.window.setVisibleOnAllWorkspaces(true);
        if (isDev) {
            this.window.webContents.openDevTools();
            this.window.loadURL('http://localhost:4200/');
        } else {
            this.window.loadURL(url.format({
                pathname: path.join(__dirname, '../../dist/index.html'),
                protocol: 'file:',
            }));
        }
        this.window.webContents.once('dom-ready', () => {
            this.window.webContents.send('navigate-to-route', '/dropdown/cpu');
        });
        return this.window;
    }

    private hideWindow() {
        this.window.hide();
    }

    private showWindow(bounds: Rectangle) {
        this.window.setPosition(bounds.x, bounds.y);
        this.window.show();
    }

    private onClick(e: Event, bounds: Rectangle) {
        if (this.window.isVisible()) {
            return this.hideWindow();
        }
        this.showWindow(bounds);
    }
}


export class StatusBar {
    private items: StatusBarItem[] = [];

    constructor() {
        const item = new StatusBarItem();
        this.items.push(item);
    }
}
