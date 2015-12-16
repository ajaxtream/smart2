'use strict';

var jobSchedule = require('./jobscheduling/build/Release/jobSchedule');
var GoogleMapsAPI = require('googlemaps');
var googleMapConfig = require('../../../../googlemapConfig');
var gmAPI = new GoogleMapsAPI( googleMapConfig );
//var db = require('../../mongodb.js');
var MAX_N_TASKS = 10;

/*
 For a controller in a127 (which this is) you should export the functions referenced in your Swagger document by name.
 Either:
  - The HTTP Verb of the corresponding operation (get, put, post, delete, etc)
  - Or the operationId associated with the operation in your Swagger document
 */
module.exports = {
  postSchedule: postSchedule
};

function setTestCase()
{
	var locations = [ 
		{Lat:51.078302, Lng:-114.130180}, //UC
		{Lat:51.059634, Lng:-114.095289}, //RST
		{Lat:51.059767, Lng:-113.985789}, //T&T
		{Lat:51.037614, Lng:-114.185730} //LRT
	];
	var start_location = locations[0];
	var task_list = [
		{tid:"eat", "location":locations[1], deadline:null, ready_time:null, depend:null},
		{tid:"buy", "location":locations[2], deadline:null, ready_time:null, depend:null},
		{tid:"work", "location":locations[3], deadline:null, ready_time:null, depend:null}
	];
	var start_time = null;
	var googlemap_option = {
		mode : 'driving',
		avoid : 'tolls' 
	};
	return {"task_list": task_list, "start_location": start_location, "start_time":start_time, "googlemap_option":googlemap_option};
}

function setGMparas(addrs, opt)
{
	var para = { 
		origins: addrs.join('|'), 
		destinations: addrs.join('|'),
       	};
	if (opt && opt.avoid)
		para.avoid = opt.avoid;
	if (opt && opt.mode)
		para.mode = opt.mode;
	return para;
}

function postSchedule(req, res) {
	// variables defined in the Swagger document can be referenced using req.swagger.params.{parameter_name}
	var inputPara = req.swagger.params.inputArg.value;

	inputPara = setTestCase();

	var largeInf = 1e20;
	var task_list = inputPara.task_list;
	var start_location = inputPara.start_location;
	var start_time = inputPara.start_time;
	var googlemap_option = inputPara.googlemap_option;
	//console.log(JSON.stringify(task_list));
	if (task_list.length > MAX_N_TASKS) {
		var ret = {"err_msg":"Too Many Tasks", "schedule":null};
		res.json(JSON.stringify(ret));
		return;
	}
	for (var i=0; i<task_list.length; i++) {
		task_list[i].location =  task_list[i].location.Lat + ',' + task_list[i].location.Lng;
		task_list[i].did = null;
	}
	var addrs = [];
	addrs[0] = start_location.Lat + "," + start_location.Lng;
	for (var i=0; i<task_list.length; i++)
		addrs[i+1] = task_list[i].location;
	var gmParams = setGMparas(addrs, googlemap_option);
	//db.log(gmParams);
	gmAPI.distance(gmParams, function(err, distMatrix){
		if (err != null) {
			console.log("error:" + err);
			res.json("Error: Query Google Maps Distance Matrix returns:<br>" + err); 
			return;
		}
		//db.log(distMatrix);
		//console.log(JSON.stringify(distMatrix));
		var matrixStr = 'Distance Matrix:<br>';
		var paths = [];
		for (var i=0; i<addrs.length; i++) { 
			for (var j=0; j<addrs.length; j++) 
				if (j!=i){
					var dist = distMatrix.rows[i].elements[j].duration.value;
					matrixStr += ' ' + dist;
					paths.push({"start":addrs[i], "end":addrs[j], "time": dist });
				} 
				else matrixStr += ' 0'; 
			matrixStr += '<br>';
		}
		console.log(matrixStr);
		var driver = { "did": ":)", "curtask": null, 
			"available": start_time, 
			"location": addrs[0], 
			"off": null };
		// algOpt==1 : optimal algorithm; algOpt == 0: greedy algorithm
		var paras = JSON.stringify({"algOpt":1, 
			"curTime":start_time, 
			"drivers":[driver], 
			"tasks":task_list, 
			"paths":paths});
		//console.log(paras); 
		jobSchedule.search(paras, function(schedule){ 
			var ret = JSON.parse(schedule); 
			var result = { err_msg:null, schedule:null};
			//var result; //an interesting error will happen if you uncomment this line to replace the above one
			if (ret.err_msg != undefined && ret.err_msg != null)
				result.err_msg = ret.err_msg;
			else {
				var taskList = ret.schedules[0].tids;
				//var message = matrixStr + "Suggested schedule: <br>" + scheduleToString(taskList, paths, curLoc) + "<br>";
				console.log(taskList.join(','));
				result.schedule = taskList.join(',');
			}
			res.json(JSON.stringify(result));
			return ;
		});
	});
}
