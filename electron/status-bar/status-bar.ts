import { Tray, BrowserWindow, Rectangle } from 'electron';
import * as path from 'path';
import { Subject } from 'rxjs';
import * as url from 'url';

import { CPUMonitor } from '../stats/cpu';
import { CPUData } from '../../src/interfaces/cpu';

const isDev = require('electron-is-dev');


class StatusBarItem {
    private tray: Tray;
    private window: BrowserWindow;
    private monitor = new CPUMonitor();

    private update$ = new Subject<CPUData>();

    constructor() {
        this.createWindow();
        this.createTray();
        this.monitor.data.subscribe(data => {
            this.window.webContents.send('cpu-data', data);
            const title = `CPU: ${Math.round(data.totalUsage.userCPU + data.totalUsage.systemCPU)}%`;
            this.tray.setTitle(title);
            this.update$.next(data);
        });
    }

    private createTray() {
        this.tray = new Tray(path.join(__dirname, '..', 'assets', 'images', 'magic-lamp_16x16.png'));
        this.tray.setToolTip('MacGenius');
        this.tray.on('click', this.onClick.bind(this));
    }

    private createWindow() {
        this.window = new BrowserWindow({
            resizable: false,
            show: false,
            frame: false,
            width: 1000,  // 280
            height: 1000,
        });
        this.window.on('blur', () => {
            this.window.hide();
        });
        this.window.setVisibleOnAllWorkspaces(true);
        this.window.webContents.openDevTools();
        if (isDev) {
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
