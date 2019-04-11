import { Tray, Rectangle, BrowserWindow } from 'electron';
import * as path from 'path';
import { Subject } from 'rxjs';
import { throttleTime } from 'rxjs/operators';

import { CPUMonitor } from '../stats/cpu';
import { CPUData } from '../../interfaces/cpu';


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
        this.tray = new Tray(path.join(__dirname, '..', 'assets', 'images', 'icon.png'));
        this.tray.setToolTip('MacGenius.');
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
        this.window.loadURL('http://localhost:4200/dropdown/cpu');
        this.window.webContents.openDevTools();
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
