/**
 * XadillaX created at 2014-12-15 18:44:02
 *
 * Copyright (c) 2014 Huaban.com, all rights
 * reserved
 */
var exec = require("child_process").exec;
var run = require("sync-runner");

var TEST_TIME = 50;
var regexp = /.*?(\d+).*\n.*?(\d+).*\n.*?(\d+).*\n.*?(\d+)/;

exec("make perf_xmem && make perf_alloc", function(err) {
    if(err) {
        console.error("Error occurred while compling: " + err.message);
        process.exit(4);
    }

    var results = [];
    var cases = [ "./perf_xmem", "./perf_alloc" ];

    for(var i = 0; i < cases.length; i++) {
        var test = {
            times   : 0,
            alloc   : 0,
            free    : 0,
            total   : 0
        };

        for(var j = 0; j < TEST_TIME; j++)
        {
            var result  = run(cases[i]);
            result      = regexp.exec(result);
            test.times += Number(result[1]);
            test.alloc += Number(result[2]);
            test.free  += Number(result[3]);
            test.total += Number(result[4]);
        }

        for(var key in test) {
            if(!test.hasOwnProperty(key)) continue;
            test[key] /= TEST_TIME;
        }

        results.push(test);
    }

    for(var j = 0; j < cases.length; j++) {
        console.log("Performance of \"" + cases[j] + "\"");
        console.log("  Test cases: " + TEST_TIME);
        console.log("  Times of each case: " + results[j].times);
        console.log("  Average of alloc in one case: " + results[j].alloc + " CPU cycles");
        console.log("  Average of free in one case: " + results[j].free + " CPU cycles");
        console.log("  Avarage of total in one case: " + results[j].total + " CPU cycles");
        console.log();
    }
});

