'use strict';

const { exec, execSync } = require('child_process');
const fs = require('fs');

const TEST_TIME = 10;
const regexp = /.*?(\d+).*\n.*?(\d+).*\n.*?(\d+).*\n.*?(\d+)/;

exec('make perf_xmem && make perf_alloc', function(err) {
  if (err) {
    console.error('Error occurred while compling: ' + err.message);
    process.exit(4);
  }

  const times = [ 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 ];
  const cases = [ './perf_xmem', './perf_alloc' ];
  const result = {};

  for (let i = 0; i < cases.length; i++) {
    console.log('testing `' + cases[i] + '`...');
    result[cases[i]] = [];
    for (let j = 0; j < times.length; j++) {
      console.log('  testing `' + cases[i] + ' ' + times[j] + '`...');
      const test = {
        times: 0,
        alloc: 0,
        free: 0,
        total: 0,
      };

      for (let k = 0; k < TEST_TIME; k++) {
        console.log(`testing \`${cases[i]} ${times[j]} for ${k} times...\``);
        const stdout = execSync(cases[i] + ' ' + times[j]);
        const output = regexp.exec(stdout);
        test.times += Number(output[1]);
        test.alloc += Number(output[2]);
        test.free += Number(output[3]);
        test.total += Number(output[4]);
        console.log('  done.');
      }

      for (const key in test) {
        if (!test.hasOwnProperty(key)) continue;
        test[key] /= TEST_TIME;
      }

      result[cases[i]].push(test);
      console.log('  done.');
    }
    console.log('done.');
  }

  // eslint-disable-next-line node/prefer-promises/fs
  fs.writeFile(
    `./perf/report/${Date.now()}.json`,
    JSON.stringify(result),
    { encoding: 'utf8' },
    function(err) {
      if (err) {
        console.error('error occurred: ' + err.message);
      } else {
        console.log('😊 all done!');
      }
    });
});
