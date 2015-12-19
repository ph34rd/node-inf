node-inf -- Inf file parser command line util
==============================

## IMPORTANT

**Only windows supported.
Working node-gyp required.**

## Install

```sh
npm install
npm run build
```

## Run

Run on local inf file with `-i` option:

```sh
npm dist/index.js -i "path/to/file.inf"
```

Run on remote zip archive with `-d` option:

```sh
npm dist/index.js -i "http://host/path/to/archive.zip"
```
