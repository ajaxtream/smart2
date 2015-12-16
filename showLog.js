var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');

var url = 'mongodb://localhost:27017/test';
MongoClient.connect(url, function(err, db) {
	  if (null !=  err)
		  console.log(err);
	  else {
		  console.log("Connected correctly to server.");
		  showRecord(db,function (){ db.close();});
	  }
});

var showRecord = function(db, callback) {
	var cursor =db.collection('querylog').find( );
	cursor.each(function(err, doc) {
		assert.equal(err, null);
		if (doc != null) {
			console.dir(doc);
		} else {
			console.log("end");
			callback();
		}
	});
};
