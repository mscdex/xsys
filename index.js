var fs = require('./build/Release/fs');

exports.fs.getFreeSync = function(path) {
  if (!path || typeof path !== 'string')
    path = __dirname;
  return fs.getFree(path);
};
exports.fs.getTotalSync = function(path) {
  if (!path || typeof path !== 'string')
    path = __dirname;
  return fs.getTotal(path);
};
exports.fs.getUsedSync = function(path) {
  if (!path || typeof path !== 'string')
    path = __dirname;
  return fs.getUsed(path);
};
exports.fs.getFree = function(path, cb) {
  if (typeof path === 'function') {
    cb = path;
    path = undefined;
  }
  if (typeof cb !== 'function')
    cb = function() {};
  if (!path)
    path = __dirname;
  return fs.getFree(path, cb);
};
exports.fs.getTotal = function(path, cb) {
  if (typeof path === 'function') {
    cb = path;
    path = undefined;
  }
  if (typeof cb !== 'function')
    cb = function() {};
  if (!path)
    path = __dirname;
  return fs.getTotal(path, cb);
};
exports.fs.getUsed = function(path, cb) {
  if (typeof path === 'function') {
    cb = path;
    path = undefined;
  }
  if (typeof cb !== 'function')
    cb = function() {};
  if (!path)
    path = __dirname;
  return fs.getUsed(path, cb);
};