import 'source-map-support/register'; // enable source maps stack traces
import bluebird from 'bluebird';

// replace native Promise with bluebird wrapper
global.Promise = bluebird.Promise;
