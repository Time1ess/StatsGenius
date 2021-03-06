import { Injectable, NgZone } from '@angular/core';
import { Router } from '@angular/router';

// If you import a module but never use any of the imported values other than as TypeScript types,
// the resulting javascript file will look as if you never imported the module at all.
import { ipcRenderer, webFrame, remote } from 'electron';
import * as childProcess from 'child_process';
import * as fs from 'fs';

@Injectable({
  providedIn: 'root',
})
export class ElectronService {

  ipcRenderer: typeof ipcRenderer;
  webFrame: typeof webFrame;
  remote: typeof remote;
  childProcess: typeof childProcess;
  fs: typeof fs;

  constructor(
    private router: Router,
    private zone: NgZone,
  ) {
    // Conditional imports
    if (this.isElectron()) {
      this.ipcRenderer = window.require('electron').ipcRenderer;
      this.webFrame = window.require('electron').webFrame;
      this.remote = window.require('electron').remote;

      this.childProcess = window.require('child_process');
      this.fs = window.require('fs');

      // Listen events from main.
      this.ipcRenderer.on('navigate-to-route', (event, url) => {
        this.zone.run(() => {
          this.router.navigateByUrl(url);
        });
      });
    }
  }

  isElectron = () => {
    return window && window.process && window.process.type && window.require;
  }

}
