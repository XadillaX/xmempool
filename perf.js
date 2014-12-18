/**
 * XadillaX created at 2014-12-15 18:44:02
 *
 * Copyright (c) 2014 Huaban.com, all rights
 * reserved
 */
var exec = require("child_process").exec;
var run = require("sync-runner");
var fs = require("fs");

var TEST_TIME = 10;
var regexp = /.*?(\d+).*\n.*?(\d+).*\n.*?(\d+).*\n.*?(\d+)/;

exec("make perf_xmem && make perf_alloc", function(err) {
    if(err) {
        console.error("Error occurred while compling: " + err.message);
        process.exit(4);
    }

    var times = [ 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 ];
    var cases = [ "./perf_xmem", "./perf_alloc" ];
    var result = {};

    for(var i = 0; i < cases.length; i++) {
        console.log("testing `" + cases[i] + "`...");
        result[cases[i]] = [];
        for(var j = 0; j < times.length; j++) {
            console.log("  testing `" + cases[i] + " " + times[j] + "`...");
            var test = {
                times : 0,
                alloc : 0,
                free  : 0,
                total : 0
            };

            for(var k = 0; k < TEST_TIME; k++) {
                console.log("    testing `" + cases[i] + " " + times[j] + "` for " + k + " times...");
                var stdout  = run(cases[i] + " " + times[j]);
                var output  = regexp.exec(stdout);
                test.times += Number(output[1]);
                test.alloc += Number(output[2]);
                test.free  += Number(output[3]);
                test.total += Number(output[4]);
                console.log("    done.");
            }

            for(var key in test) {
                if(!test.hasOwnProperty(key)) continue;
                test[key] /= TEST_TIME;
            }

            result[cases[i]].push(test);
            console.log("  done.");
        }
        console.log("done.");
    }

    fs.writeFile("./perf/report/" + (+new Date()) + ".json", JSON.stringify(result), { encoding: "utf8" }, function(err) {
        if(err) {
            console.error("error occurred: " + err.message);
        } else {
            console.log("😊 all done!");
        }
    });
});

