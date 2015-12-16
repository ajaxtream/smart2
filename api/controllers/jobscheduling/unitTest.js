var jobSchedule = require('./build/Release/jobSchedule');

var drivers = [
    {
        "did": "23",
	"curtask": null,
        "available": 0,
        "location": "Chanmao Inc.",
        "off": null
    }
];

var tasks = 
[ 
{ "tid":"order1-fetch", "location":"Chanmao Inc.", "deadline":1432519683235, "did":"", "depend":null }, 
{ "tid":"order1-deliver", "location":"Client1", "deadline":1432519683235, "did":"", "depend":"order1-fetch" }, 
{ "tid":"order2-deliver", "location":"Client2", "deadline":1432519683235, "did":"", "depend":"" } 
];

var paths = 
[ 
{ "start":"Chanmao Inc.", "end":"Client1", "time":10},
{ "end":"Chanmao Inc.", "start":"Client1", "time":10},
{ "start":"Chanmao Inc.", "end":"Client2", "time":20},
{ "end":"Chanmao Inc.", "start":"Client2", "time":20},
{ "start":"Client1", "end":"Client2", "time":25},
{ "end":"Client1", "start":"Client2", "time":25}
];


//var d = new Date();
//var curTime = d.getTime();
var curTime = 0;
var default_exe_time = 1;
//jobSchedule.search("{ test parse error", callbackFunc);
jobSchedule.search(JSON.stringify({"algOpt":1, "curTime":curTime, "default_task_time":default_exe_time, "drivers":drivers, "tasks":tasks, "paths":paths}), function callbackFunc(str) {
	console.log(str);
}
);
//console.log(jobSchedule.search(JSON.stringify({"drivers":drivers, "tasks":tasks, "paths":paths})));
