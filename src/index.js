import './bootstrap';
import parse from 'parsec';
import Download from 'download';
import infUtils from './infUtils';
import _rimraf from 'rimraf';
import path from 'path';
import walk from 'walk';

// promisify modules
Download.prototype.run = Promise.promisify(Download.prototype.run);
const rimraf = Promise.promisify(_rimraf);

function localFile(resolved) {
  console.log(`input: "${resolved}"`);
  const result = JSON.stringify(infUtils.info(resolved));
  console.log(`result: ${result}`);
}

// walk wrapper
function wrapWalker(tmpDir, cb) {
  const w = walk.walk(tmpDir, { followLinks: false });

  w.on('errors', function(root, nodeStatsArray, next) {
    next();
  });

  w.on('file', function(root, fileStats, next) {
    if (fileStats.name.match(/.inf$/i)) {
      const infFile = path.join(root, fileStats.name);

      localFile(infFile); // check single file
    }

    next();
  });

  w.on('end', function() {
    cb();
  });
}
const walker = Promise.promisify(wrapWalker);

function tmpName() {
  const alpha = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
  let result = '';

  for (let i = 0; i < 16; i++) {
    result += alpha[Math.floor(Math.random() * alpha.length)];
  }

  return result;
}

async function downloadUnzip(url, tmpDir) {
  console.log(`downloading: ${url}`);
  console.log(`tmp: ${tmpDir}`);

  await new Download({ extract: true }).get(url).dest(tmpDir).run();
  await walker(tmpDir);
}

// main
const argv = parse();

if (argv.i) {
  const fileName = argv.i.toString();
  const resolved = path.resolve(fileName);

  localFile(resolved);
} else if (argv.d) {
  const url = argv.d.toString();
  const tmpDir = path.resolve(tmpName());

  downloadUnzip(url, tmpDir).catch(function(e) {
    console.log(`error: ${e.message}`);
  }).finally(async function() {
    await rimraf(tmpDir);
  });
}
