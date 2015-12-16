#include <iostream>
#include <stdlib.h>
#include "jobSchedule.h"
using namespace std;

void printSchedule(vector<CScheduleItem> &schedule);


bool checkConstraints(vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, vector<CScheduleItem> &schedule) 
{
	//check duplicate tasks in Schedule
	//check duplicate driver in Schedule 
	vector<int> task_count(tasks.size(), 0);
	vector<int> driver_count(drivers.size(), 0);
	vector< vector<int> > s(drivers.size(), vector<int>(tasks.size(),-1));
	vector<int> task_to_driver(tasks.size(), -1);
	for (int i=0; i<schedule.size(); i++) {
		vector<CID> &tasklist = schedule[i].tids;
		bool found = false;
		int iDriver = -1;
		for (int j=0; j<drivers.size(); j++) {
			if (schedule[i].did == drivers[j].did) {
				driver_count[j]++;
				iDriver = j;
				found = true;
				break;
			}
		}
		if (!found) {
			cout<< "returned unknown driverID:"<< schedule[i].did << endl;
			return false;
		}
		for (int k=0; k<tasklist.size(); k++) {
			found = false;
			for (int j=0; j<tasks.size(); j++)
				if (tasks[j].tid == tasklist[k]) {
					s[iDriver][k] = j;
					task_to_driver[j] = iDriver;
					found = true;
					task_count[j]++;
					break;
				}
			if (!found) {
				cout<< "returned unknown taskID:"<< tasklist[k] << endl;
				return false;
			}
		}
	}
	for (int i=0; i<tasks.size(); i++) 
		if (task_count[i] > 1) {
			cout<< "duplicate task in schedule: "<< tasks[i].tid << endl;
			return false;
		}
	for (int i=0; i<drivers.size(); i++) 
		if (driver_count[i] > 1) {
			cout<< "duplicate driver in schedule: "<< drivers[i].did << endl;
			return false;
		}
	//check asgnDriver and  prevTask
	for (int i=0; i<tasks.size(); i++) {
		int iDriver = task_to_driver[i];
		if (iDriver == -1) {
			cout << "Unassigned task " << tasks[i].tid << endl;
			continue;
		}
		if (tasks[i].did != NULL_ID) {
			if (tasks[i].did != drivers[iDriver].did) {
				cout<< "task "<< tasks[i].tid << "assigned to "<< drivers[iDriver].did << ", pre-assigned to : " << tasks[i].did<< endl;
				return false;
			}
		} 
		if (tasks[i].depend != NULL_ID) {
			int iPrevTask = -1;
			for (int j=0; j<tasks.size(); j++) 
				if (tasks[j].tid == tasks[i].depend) {
					iPrevTask = j;
					break;
				}
			if (iPrevTask == -1) {
				cout << "Input error. Cannot find prevTask " << tasks[i].depend << endl;
				return false;
			}
			if (task_to_driver[iPrevTask] != iDriver) {
				cout << "Fetch and deliver of an order are assigned to different drivers. " << iDriver << task_to_driver[iPrevTask] << endl;
				return false;
			}
			int idx1, idx2;
			idx1 = idx2 = -1;
			for (int j=0; j<tasks.size(); j++) {
				int k = s[iDriver][j];
				if (k<0) break;
				if (k == iPrevTask) 
					idx1 = k;
				else if (k== i)
					idx2 = k;
			}
			if (idx1 >= idx2) {
				cout << "Fetch and deliver disorder.  " << idx1 << " " << idx2 << endl;
				return false;
			}
		}
	}
	//check driver's off work time
	for (int i=0; i<drivers.size(); i++) {
		CTime t = drivers[i].available;
		CLocationID venue = drivers[i].location;
		for(int j=0; j<tasks.size(); j++) {
			int k = s[i][j];
			if (k<0) break;
			if (venue == tasks[k].location) continue;
			CRTime deltT;
			bool found = false;
			for (int o=0; o<paths.size(); o++) 
				if (paths[o].start == venue && paths[o].end == tasks[k].location){
					deltT = paths[o].time;
					found = true;
					break;
				}
			if (!found) {
				cout<< "Cannot find a path from " << venue << " to " << tasks[k].location << endl;
				return false;
			}
			t += deltT;
			venue = tasks[k].location;
		}
		if (t>drivers[i].off) {
			cout<< "Driver " << drivers[i].did << " has to work late." << endl;
			return false;
		}
	}
	cout << "Passed feasibility check." << endl;
	return true;
}
void genRandomPath(vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths)
{
	vector<string> locations;
	vector<int> cords[2];
	for (int i=0; i<drivers.size(); i++) {
		bool found = false;
		for (int j=0; j<locations.size(); j++) 
			if (drivers[i].location == locations[j]) {
				found = true;
				break;
			}
		if (!found) 
			locations.push_back(drivers[i].location);
	}
	for (int i=0; i<tasks.size(); i++) {
		bool found = false;
		for (int j=0; j<locations.size(); j++) 
			if (tasks[i].location == locations[j]) {
				found = true;
				break;
			}
		if (!found) 
			locations.push_back(tasks[i].location);
	}
	srand(time(0));
	int n = locations.size();
	cords[0].resize(n); cords[1].resize(n);
	for (int i=0; i<n; i++) { 
		for (int ti=0; ti<2; ti++) cords[ti][i] = random() & 0x7f; //ranges 0~127
	}
	paths.clear();
	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++) if (i != j) {
			CRTime travelTime = abs(cords[0][i]-cords[0][j]) + abs(cords[1][i]-cords[1][j]);
			paths.push_back(CPath(locations[i], locations[j], travelTime)); 
		}
}
void testCase1()
	//just a simple test
{
	vector<CDriver> drivers, drivers0;
	vector<CTask> tasks, tasks0;
	vector<CPath> paths, paths0;
	vector<CScheduleItem> schedule;
	drivers.clear();
	tasks.clear();
	paths.clear();

	drivers.push_back(CDriver("Xunrui", 0, 600, "base station"));
	drivers.push_back(CDriver("Aiden", 100, 400, "Chanmao Restaurant"));

	tasks.push_back(CTask("fetch Ginger Beef", "Chanmao Restaurant", 110)); 
	tasks.push_back(CTask("deliver Ginger Beef", "Customer1", 210, 0, NULL_ID, "fetch Ginger Beef"));
	tasks.push_back(CTask("deliver KungPao Chicken", "Customer2", 310, 0, "Aiden"));

	genRandomPath(drivers, tasks, paths);
	drivers0 = drivers;
	tasks0 = tasks;
	paths0 = paths;
	bool ret;
	ret = ALG::findScheduleGreedy(0, 9000, drivers, tasks, paths, schedule);
	cout << "returned: " << ret << endl;
	checkConstraints(drivers0, tasks0, paths0, schedule);
	printSchedule(schedule);
}

void genPath_case2(vector<CPath> & paths)
{
	const int n=6;
	string locations[n] = {"Shop1", "Shop2", "Client1", "Client2", "Client3", "Client4"};
	double map[n][n];
	for (int i=0; i<n; i++) 
		for (int j=0; j<n; j++) map[i][j] = -1;
	map[0][1] = 100;
	map[0][2] = 10;
	map[0][3] = 10;
	map[1][4] = 10;
	map[1][5] = 10;
	map[2][3] = 10;
	map[4][5] = 10;
	for (int i=0; i<n; i++) 
		for (int j=i+1; j<n; j++) if (map[i][j]>0) {
			map[j][i] = map[i][j];
		}

	for (int k=0; k<n; k++)
		for (int i=0; i<n; i++) {
			if (i==k) continue;
			for (int j=0; j<n; j++) {
				if (j==i || j==k) continue;
				if (map[i][k]>=0 && map[k][j]>=0 && (map[i][j] < 0 || map[i][j] > map[i][k]+map[k][j]))
					map[i][j] = map[i][k] + map[k][j];
			}
		}
	for (int i=0; i<n; i++)
		for (int j=i+1; j<n; j++) {
			paths.push_back(CPath(locations[i],locations[j], map[i][j]));
			paths.push_back(CPath(locations[j],locations[i], map[j][i]));
		}
	return;
}

void testCase2()
{
	vector<CDriver> drivers, drivers0;
	vector<CTask> tasks, tasks0;
	vector<CPath> paths, paths0;
	vector<CScheduleItem> schedule;
	drivers.clear();
	tasks.clear();
	paths.clear();

	drivers.push_back(CDriver("Driver1", 0, 30, "Shop1"));
	drivers.push_back(CDriver("Driver2", 0, 30, "Shop1"));
	drivers.push_back(CDriver("Driver3", 0, 30, "Shop2"));

	tasks.push_back(CTask("Order1-Step1", "Shop1", 100)); 
	tasks.push_back(CTask("Order1-Step2", "Client1", 100, 0, NULL_ID, "Order1-Step1")); 
	tasks.push_back(CTask("Order2-Step1", "Shop1", 100)); 
	tasks.push_back(CTask("Order2-Step2", "Client2", 100, 0, NULL_ID, "Order2-Step1")); 
	tasks.push_back(CTask("Order3-Step1", "Shop2", 100)); 
	tasks.push_back(CTask("Order3-Step2", "Client3", 100, 0, NULL_ID, "Order3-Step1")); 
	tasks.push_back(CTask("Order4-Step1", "Shop2", 100)); 
	tasks.push_back(CTask("Order4-Step2", "Client4", 100, 0, NULL_ID, "Order4-Step1")); 

	//genRandomPath(drivers, tasks, paths);
	genPath_case2(paths);

	drivers0 = drivers;
	tasks0 = tasks;
	paths0 = paths;
	bool ret;
	ret = ALG::findScheduleGreedy(0, 9000, drivers, tasks, paths, schedule);
	cout << "returned: " << ret << endl;
	checkConstraints(drivers0, tasks0, paths0, schedule);
	printSchedule(schedule);
}

void testCase3() //after one task finished in testCase2
{
	vector<CDriver> drivers, drivers0;
	vector<CTask> tasks, tasks0;
	vector<CPath> paths, paths0;
	vector<CScheduleItem> schedule;
	drivers.clear();
	tasks.clear();
	paths.clear();

	drivers.push_back(CDriver("Driver1", 0, 30, "Shop1"));
	drivers.push_back(CDriver("Driver2", 0, 30, "Shop1"));
	drivers.push_back(CDriver("Driver3", 0, 30, "Shop2"));

	tasks.push_back(CTask("Order1-Step2", "Client1", 100, 0, "Driver1")); 
	tasks.push_back(CTask("Order2-Step1", "Shop1", 100)); 
	tasks.push_back(CTask("Order2-Step2", "Client2", 100, 0, NULL_ID, "Order2-Step1")); 
	tasks.push_back(CTask("Order3-Step1", "Shop2", 100)); 
	tasks.push_back(CTask("Order3-Step2", "Client3", 100, 0, NULL_ID, "Order3-Step1")); 
	tasks.push_back(CTask("Order4-Step1", "Shop2", 100)); 
	tasks.push_back(CTask("Order4-Step2", "Client4", 100, 0, NULL_ID, "Order4-Step1")); 

	//genRandomPath(drivers, tasks, paths);
	genPath_case2(paths);

	drivers0 = drivers;
	tasks0 = tasks;
	paths0 = paths;
	bool ret;
	ret = ALG::findScheduleGreedy(0, 9000, drivers, tasks, paths, schedule);
	cout << "returned: " << ret << endl;
	checkConstraints(drivers0, tasks0, paths0, schedule);
	printSchedule(schedule);
}
void testCase4() // same map as testCase2, but two driver cannot complete all the tasks in time
{
	vector<CDriver> drivers, drivers0;
	vector<CTask> tasks, tasks0;
	vector<CPath> paths, paths0;
	vector<CScheduleItem> schedule;
	drivers.clear();
	tasks.clear();
	paths.clear();

	drivers.push_back(CDriver("Driver1", 0, 30, "Shop1"));
	drivers.push_back(CDriver("Driver2", 0, 30, "Shop1"));
	drivers.push_back(CDriver("Driver3", 0, 30, "Shop2"));

	tasks.push_back(CTask("Order1-Step1", "Shop1", 100)); 
	tasks.push_back(CTask("Order1-Step2", "Client1", 10, 0, NULL_ID, "Order1-Step1")); 
	tasks.push_back(CTask("Order2-Step1", "Shop1", 100)); 
	tasks.push_back(CTask("Order2-Step2", "Client2", 10, 0, NULL_ID, "Order2-Step1")); 
	tasks.push_back(CTask("Order3-Step1", "Shop2", 100)); 
	tasks.push_back(CTask("Order3-Step2", "Client3", 100, 0, NULL_ID, "Order3-Step1")); 
	tasks.push_back(CTask("Order4-Step1", "Shop2", 100)); 
	tasks.push_back(CTask("Order4-Step2", "Client4", 100, 0, NULL_ID, "Order4-Step1")); 

	//genRandomPath(drivers, tasks, paths);
	genPath_case2(paths);

	drivers0 = drivers;
	tasks0 = tasks;
	paths0 = paths;
	bool ret;
	ret = ALG::findScheduleGreedy(0, 9000, drivers, tasks, paths, schedule);
	cout << "returned: " << ret << endl;
	checkConstraints(drivers0, tasks0, paths0, schedule);
	printSchedule(schedule);
}
int main()
{
	testCase4();
	return 0;
	testCase1();
	testCase2();
	testCase3();
	return 0;
}
void printSchedule(vector<CScheduleItem> &schedule)
{
	for (vector<CScheduleItem>::iterator it = schedule.begin(); it<schedule.end(); it++) {
		vector<CID> &tasklist = it->tids;
		cout << "Driver:" << it->did << " is assigned with "<< tasklist.size() << " tasks:" << endl;
		for (int i=0; i<tasklist.size(); i++) {
			cout << "  " << tasklist[i] << "; ";
		}
		cout << endl;
	}
}
