"use strict";
exports.__esModule = true;
var rxjs_1 = require("rxjs");
var genius_1 = require("./genius");
rxjs_1.interval(1000).subscribe(function () {
    console.log(genius_1.processes().map(function (x) { return x.commandName; }));
});
