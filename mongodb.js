var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');

var url = 'mongodb://localhost:27017/test';
var db = null;

MongoClient.connect(url, function(err, connected_db) {
	  if (null !=  err) {
		  console.log(err);
		  return;
	  }
	  else {
		  console.log("Connected to mongodb server.");
		  db = connected_db;
	  }
});

var showAllRecord = function() {
	var cursor =db.collection('querylog').find( );
	cursor.each(function(err, doc) {
		assert.equal(err, null);
		if (doc != null) {
			console.dir(doc);
		} else {
			console.log("end");
		}
	});
};
var logRecord = function(record) {
	db.collection('querylog').insertOne(record, function(err, result){if (err) console.log(err);});
};

exports.log = logRecord;
exports.showAll = showAllRecord;
exports.close = function () { if (db) db.close();}
