var addon = require('./build/Release/addon');

var drivers = [
{ "ID": "driver1", "avlbTime": 10, "offTime": 100, "avlbLocaion": "shop1" },
{ "ID": "driver2", "avlbTime": 10, "offTime": 100, "avlbLocaion": "shop1" }
];

var tasks = [ 
{ "ID":"order1-fetch", "deadline":30, "venue":"shop1"}, 
{ "ID":"order1-deliver", "deadline":30, "venue":"client1"}, 
];
var paths = [ { "src":"shop1", "dst":"client1", "distance":10}];

console.log(addon.hello(JSON.stringify({"drivers":drivers, "tasks":tasks, "paths":paths})));
