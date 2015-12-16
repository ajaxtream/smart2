var jobSchedule = require('./build/Release/jobSchedule');

var drivers = [
    {
        "did": "23",
	"curtask": "0,0,loc6,stamp1",
        "available": 1433835827647,
        "location": "loc6",
        "off": null
    },
    {
        "did": "24",
	"curtask": null,
        "location": "loc6",
        "available": 1433835759562,
        "off": 1433901600000
    }
];

var tasks = [
    {
        "tid": "0,0,loc6,stamp1",
        "location": "loc6",
        "deadline": 1433839398659,
        "ready_time": 1433837598659,
        "depend": null,
        "did": null
    },
    {
        "tid": "loc6,loc1,stamp2",
        "location": "loc1",
        "deadline": 1433842998659,
        "ready_time": 0,
        "depend": "0,0,loc6,stamp1",
        "did": null
    },
    {
        "tid": "0,0,loc6,stamp3",
        "location": "loc6",
        "deadline": 1433839398662,
        "ready_time": 1433837598662,
        "depend": null,
        "did": null
    },
    {
        "tid": "loc6,loc1,stamp3",
        "location": "loc1",
        "deadline": 1433842998662,
        "ready_time": 0,
        "depend": "0,0,loc6,stamp3",
        "did": null
    },
    {
        "tid": "0,0,loc6,stamp4",
        "location": "loc6",
        "deadline": 1433839407633,
        "ready_time": 1433837607633,
        "depend": null,
        "did": null
    },
    {
        "tid": "loc6,loc3,stamp4",
        "location": "loc3",
        "deadline": 1433843007633,
        "ready_time": 0,
        "depend": "0,0,loc6,stamp4",
        "did": null
    },
    {
        "tid": "0,0,loc6,stamp5",
        "location": "loc6",
        "deadline": 1433839417596,
        "ready_time": 1433837617596,
        "depend": null,
        "did": null
    },
    {
        "tid": "loc6,loc4,stamp5",
        "location": "loc4",
        "deadline": 1433843017596,
        "ready_time": 0,
        "depend": "0,0,loc6,stamp5",
        "did": null
    },
    {
        "tid": "0,0,loc6,stamp6",
        "location": "loc6",
        "deadline": 1433839427630,
        "ready_time": 1433837627630,
        "depend": null,
        "did": null
    },
    {
        "tid": "loc6,loc2,stamp7",
        "location": "loc2",
        "deadline": 1433843027630,
        "ready_time": 0,
        "depend": "0,0,loc6,stamp6",
        "did": null
    },
    {
        "tid": "0,0,loc6,stamp8",
        "location": "loc6",
        "deadline": 1433839437659,
        "ready_time": 1433837637659,
        "depend": null,
        "did": null
    },
    {
        "tid": "loc6,loc5,stamp8",
        "location": "loc5",
        "deadline": 1433843037659,
        "ready_time": 0,
        "depend": "0,0,loc6,stamp8",
        "did": null
    }
];
//[ 
//{ "tid":"order1-fetch", "location":"Chanmao Inc.", "deadline":1432519683235, "did":"", "depend":null }, 
//{ "tid":"order1-deliver", "location":"Client1", "deadline":1432519683235, "did":"", "depend":"order1-fetch" }, 
//{ "tid":"order2-deliver", "location":"Client2", "deadline":1432519683235, "did":"Aiden", "depend":"" } 
//];

var paths = [
    {
        "start": "loc4",
        "end": "loc2",
        "time": 2083000
    },
    {
        "start": "loc5",
        "end": "loc1",
        "time": 1081000
    },
    {
        "start": "loc4",
        "end": "loc3",
        "time": 420000
    },
    {
        "start": "loc3",
        "end": "loc4",
        "time": 389000
    },
    {
        "start": "loc5",
        "end": "loc6",
        "time": 1081000
    },
    {
        "start": "loc4",
        "end": "loc1",
        "time": 1423000
    },
    {
        "start": "loc2",
        "end": "loc6",
        "time": 902000
    },
    {
        "start": "loc3",
        "end": "loc2",
        "time": 1882000
    },
    {
        "start": "loc5",
        "end": "loc4",
        "time": 1797000
    },
    {
        "start": "loc4",
        "end": "loc6",
        "time": 1423000
    },
    {
        "start": "loc6",
        "end": "loc2",
        "time": 929000
    },
    {
        "start": "loc3",
        "end": "loc6",
        "time": 1322000
    },
    {
        "start": "loc5",
        "end": "loc3",
        "time": 1594000
    },
    {
        "start": "loc1",
        "end": "loc3",
        "time": 1206000
    },
    {
        "start": "loc5",
        "end": "loc2",
        "time": 716000
    },
    {
        "start": "loc1",
        "end": "loc5",
        "time": 1100000
    },
    {
        "start": "loc2",
        "end": "loc5",
        "time": 671000
    },
    {
        "start": "loc6",
        "end": "loc3",
        "time": 1206000
    },
    {
        "start": "loc2",
        "end": "loc4",
        "time": 2011000
    },
    {
        "start": "loc6",
        "end": "loc5",
        "time": 1100000
    },
    {
        "start": "loc4",
        "end": "loc5",
        "time": 1696000
    },
    {
        "start": "loc3",
        "end": "loc5",
        "time": 1595000
    },
    {
        "start": "loc3",
        "end": "loc1",
        "time": 1322000
    },
    {
        "start": "loc6",
        "end": "loc1",
        "time": 0
    },
    {
        "start": "loc2",
        "end": "loc3",
        "time": 1842000
    },
    {
        "start": "loc1",
        "end": "loc6",
        "time": 0
    },
    {
        "start": "loc6",
        "end": "loc4",
        "time": 1409000
    },
    {
        "start": "loc1",
        "end": "loc4",
        "time": 1409000
    },
    {
        "start": "loc1",
        "end": "loc2",
        "time": 929000
    },
    {
        "start": "loc2",
        "end": "loc1",
        "time": 902000
    }
];
//[ 
//{ "start":"Chanmao Inc.", "end":"Client1", "time":10},
//{ "end":"Chanmao Inc.", "start":"Client1", "time":10},
//{ "start":"Chanmao Inc.", "end":"Client2", "time":20},
//{ "end":"Chanmao Inc.", "start":"Client2", "time":20},
//{ "start":"Client1", "end":"Client2", "time":25},
//{ "end":"Client1", "start":"Client2", "time":25}
//];

function callbackFunc(str) {
	console.log(str);
}

//var d = new Date();
//var curTime = d.getTime();
var curTime = 1433835827647;
var default_exe_time = 1;
//jobSchedule.search("{ test parse error", callbackFunc);
jobSchedule.search(JSON.stringify({"curTime":curTime, "default_task_time":default_exe_time, "drivers":drivers, "tasks":tasks, "paths":paths}), callbackFunc);
//console.log(jobSchedule.search(JSON.stringify({"drivers":drivers, "tasks":tasks, "paths":paths})));
