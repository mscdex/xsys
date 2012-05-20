
Description
===========

A [node.js](http://nodejs.org/) binding to useful system-level functions.


Requirements
============

* [node.js](http://nodejs.org/) -- v0.6.0 or newer


Install
============

npm install xsys


Examples
========

* Get filesystem total/free/used byte counts:
```javascript
  var fs = require('xsys').fs;

  // format our numerical output
  function fmtBytes(val) {
      return val.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ',');
  }

  fs.getTotal(function(err, n) {
      if (err) throw err;
      console.log('Total bytes for current volume: ' + fmtBytes(n));
  });
  fs.getFree(function(err, n) {
      if (err) throw err;
      console.log('Free bytes for current volume: ' + fmtBytes(n));
  });
  fs.getUsed(function(err, n) {
      if (err) throw err;
      console.log('Used bytes for current volume: ' + fmtBytes(n));
  });

  // sample output:
  //
  // Total bytes for current volume: 974,656,999,424
  // Free bytes for current volume: 877,061,001,216
  // Used bytes for current volume: 48,814,333,952
```


API
===

Static Methods
--------------

* **fs.getTotal**([<_String_>path,] <_Function_>callback) - _(void)_ - Retrieves the number of total bytes on the volume where *path* is located (defaults to path of the module). The callback receives two arguments: an <_Error_> object in case of error (null otherwise) and a <_String_> containing the number of bytes.

* **fs.getTotalSync**([<_String_>path]) - <_String_> - Synchronous version of getTotal().

* **fs.getFree**([<_String_>path,] <_Function_>callback) - _(void)_ - Retrieves the number of free bytes on the volume where *path* is located (defaults to path of the module). The callback receives two arguments: an <_Error_> object in case of error (null otherwise) and a <_String_> containing the number of bytes.

* **fs.getFreeSync**([<_String_>path]) - <_String_> - Synchronous version of getFree().

* **fs.getUsed**([<_String_>path,] <_Function_>callback) - _(void)_ - Retrieves the number of used bytes on the volume where *path* is located (defaults to path of the module). The callback receives two arguments: an <_Error_> object in case of error (null otherwise) and a <_String_> containing the number of bytes.

* **fs.getUsedSync**([<_String_>path]) - <_String_> - Synchronous version of getUsed().
