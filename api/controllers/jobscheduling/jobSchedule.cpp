#include <stdio.h>
#include "jobSchedule.h"

double ALG::map[MAXNLOCATIONS][MAXNLOCATIONS];
CRTime ALG::linger = 0;

CTime calNextAvailable(CTime tArrive, CTime tReady, CRTime linger)
{
	if (tReady > 0) 
		return (tArrive > tReady) ? tArrive: tReady;
	else
		return tArrive + linger;
}

void ALG::calDFTime(int iStart, int iEnd, CDriver & driver, vector<CTask> & tasks, int curLoc, CTime & dTime, CTime & fTime)
{
	for (int i=iStart; i<iEnd; i++) {
		int iNext = driver.tasksAtHand[i];
		fTime += map[curLoc][tasks[iNext].iVenue];
		fTime = calNextAvailable(fTime, tasks[iNext].ready, linger);
		curLoc = tasks[iNext].iVenue;
		if (fTime > tasks[iNext].deadline) 
			dTime += fTime - tasks[iNext].deadline;
	}
}
double ALG::tryAddTask(int iTask, int iDriver, CDriver & driver, vector<CTask> & tasks)
	//return the evaluation, the higher the better
{
	CTime fTime= driver.available;
	CTime dTime=0;
	//calculate the new finish time and delay time
	int curLoc = driver.iLocation;
	fTime += map[curLoc][tasks[iTask].iVenue];
	fTime = calNextAvailable(fTime, tasks[iTask].ready, linger);
	curLoc = tasks[iTask].iVenue;
	if (fTime > tasks[iTask].deadline) dTime += fTime - tasks[iTask].deadline;
	if (tasks[iTask].iNextTask == -1) {
		calDFTime(0, driver.tasksAtHand.size(), driver, tasks, curLoc, dTime, fTime); 
	}
	else {
		int introTask = tasks[iTask].iNextTask;
		CTime bestDTime=0, bestFTime;
		bestFTime = fTime + map[curLoc][tasks[introTask].iVenue];
		if (bestFTime > tasks[introTask].deadline) 
			bestDTime += bestFTime - tasks[introTask].deadline;
		bestFTime = calNextAvailable(bestFTime, tasks[introTask].ready, linger);
		calDFTime(0, driver.tasksAtHand.size(), driver, tasks, tasks[introTask].iVenue, bestDTime, bestFTime);
		for (unsigned int i=0; i<driver.tasksAtHand.size(); i++) {
			CTime tempd, tempf;
			tempf = fTime; tempd = dTime;
			calDFTime(0, i+1, driver, tasks, curLoc, tempd, tempf);
			int iLoc = tasks[driver.tasksAtHand[i]].iVenue;
			tempf += map[iLoc][tasks[introTask].iVenue];
			if (tempf > tasks[introTask].deadline) 
				tempd += tempf - tasks[introTask].deadline;
			tempf = calNextAvailable(tempf, tasks[introTask].ready, linger);
			iLoc = tasks[introTask].iVenue;
			calDFTime(i+1, driver.tasksAtHand.size(), driver, tasks, iLoc, tempd, tempf);
			if (tempf > driver.off) continue;
			if (driver.delayTime <=0 && tempd > 0) continue;
			if (bestFTime > driver.off || bestDTime > tempd) {
				bestFTime = tempf;
				bestDTime = tempd;
			}
		}
		fTime = bestFTime;
		dTime = bestDTime;
	}


	//printf("try assign task %d to dirver %d.\n", iTask, iDriver);
	if (fTime > driver.off) {
		//printf("infeasible because of offwork time\n");
		return 0;
	}
	if (driver.delayTime <= 0 && dTime > 0) {
		//printf("infeasible because this would cause jobs at hand delayed\n");
		return 0;
	}
	double delta = dTime - driver.delayTime;
	if (fTime < 0.01) fTime = 0.01;
	if (delta <= 0.01) 
		return (100/0.01) + 100/fTime;
	else
		return 100/delta;
}
void ALG::arrange_future_tasks(CDriver & driver, vector<CTask> & tasks)
{
	driver.finishTime = 0;
	driver.delayTime = 0;
	CTime curTime = driver.available;
	unsigned int n=driver.tasksAtHand.size();
	vector<bool> mark(n, false);
	vector<int> tempList(n, -1);
	int curLoc = driver.iLocation;
	for (unsigned int i=0; i<n; i++) {
		int nextj = -1;
		CTime bestTime = -1; //find the nearest as the next waypoint
		for (unsigned int j=0; j<n; j++) if (!mark[j]) {
			int iTask = driver.tasksAtHand[j];
			if (tasks[iTask].iPrevTask != -1 && tasks[tasks[iTask].iPrevTask].iDriver == -1) {
				//assert tasks[tasks[iTask].iPrevTask].iAsgnDriver == this Driver)
				bool bFeasible = true;
				int iPrevTask = tasks[iTask].iPrevTask;
				for (unsigned int k=0; k<n; k++)
					if (iPrevTask == driver.tasksAtHand[k]) {
						bFeasible = mark[k];
						break;
					}
				if (!bFeasible) continue;
			}
			if (bestTime < 0 || bestTime > map[curLoc][tasks[iTask].iVenue]) {
				bestTime = map[curLoc][tasks[iTask].iVenue];
				nextj = j;
			}
		}
		mark.at(nextj) = true;
		int iTask = driver.tasksAtHand[nextj];
		tempList[i] = iTask;
		//now go to nextTask
		curTime += map[curLoc][tasks[iTask].iVenue];
		curLoc = tasks[iTask].iVenue;
		if (tasks[iTask].deadline < curTime)
			driver.delayTime += curTime - tasks[iTask].deadline;
		curTime = calNextAvailable(curTime, tasks[iTask].ready, linger);
	}
	driver.finishTime = curTime;
	for (unsigned int i=0; i<n; i++) 
		driver.tasksAtHand[i] = tempList[n-i-1];
	return;
}
bool ALG::select_next_task(CDriver & driver, int iDriver, vector<CTask> & tasks, int & iSelectedTask)
	// return if a new job is selected
{
	iSelectedTask = -1;
	double bestEva = 0;
	for (unsigned int j=0; j<tasks.size(); j++) if (tasks[j].iDriver == -1) { //for each not fixed tasks
		if (tasks[j].iPrevTask>=0 && tasks[tasks[j].iPrevTask].iDriver == -1)
			continue;
		if (tasks[j].iAsgnDriver>=0) { 
			continue; //this task is a driver's furture task
		}
		double evaluation = tryAddTask(j, iDriver, driver, tasks);
		if (evaluation > bestEva) {
			bestEva = evaluation;
			iSelectedTask = j;
		}
	}
	if (iSelectedTask == -1 && driver.tasksAtHand.size()>0) {
		iSelectedTask = driver.tasksAtHand[driver.tasksAtHand.size()-1];
	}
	return (iSelectedTask != -1);
}
//assume each task has only one prevTask or nextTask
int ALG::findScheduleGreedy(CTime curTime, CRTime deliLimit, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, vector<CScheduleItem> &schedule)
{
	int nLocations;
	linger = deliLimit;
	int ret = preProcess(curTime, drivers, tasks, paths, nLocations);
	if (ret != E_NORMAL) 
		return ret;
	vector<int> prior(drivers.size());
	for (unsigned int i=0; i<drivers.size(); i++) prior[i] = i;
	for (unsigned int i=0; i<drivers.size(); i++) {
		for (unsigned int j=i+1; j<drivers.size(); j++)
			if (drivers[prior[i]].tasksAtHand.size() < drivers[prior[j]].tasksAtHand.size()) 
				std::swap(prior[i],prior[j]);
	}
	//use the driver with non-empty tasksAtHand first
	for (unsigned int ti=0; ti<drivers.size(); ti++) 
	{
		int i = prior[ti];
		//try to assign as much job to the driver as possible
		while (1)
		{
			int j;
			if (!select_next_task(drivers[i], i, tasks, j)) break;
			assignTaskToDriver(j, i, drivers, tasks);
		}
	}
	//set up schedule according to each driver's taskList
	schedule.clear();
	for (unsigned int i=0; i<drivers.size(); i++) 
	{
		CScheduleItem si;
		si.did = drivers[i].did;
		si.off = drivers[i].off;
		si.tids.clear();
		for (unsigned int j=0; j<drivers[i].taskList.size(); j++) {
			int idx = drivers[i].taskList[j];
			si.tids.push_back(tasks[idx].tid);
		}
		if (drivers[i].origin_available <= curTime && drivers[i].taskList.size() > 0) {
			int idx = drivers[i].taskList[0];
			si.available = curTime + map[drivers[i].iOrigin_location][tasks[idx].iVenue]; 
			si.available = calNextAvailable(si.available, tasks[idx].ready, linger);
			si.location = tasks[idx].location;
			si.updated = true;
		}
		else {
			si.location = drivers[i].location;
			si.available = drivers[i].origin_available;
			si.updated = false;
		}
		schedule.push_back(si);
	}

	return E_NORMAL;
}
void ALG::assignTaskToDriver(int iTask, int iDriver, vector<CDriver> & drivers, vector<CTask> & tasks)
{
	//printf("assign task %d to driver %d\n", iTask, iDriver);
	drivers[iDriver].taskList.push_back(iTask);
	if (tasks[iTask].iDriver != -1) {
		fprintf(stderr, "internal error while assigning task to driver! task already assigned.\n");
		return;
	}
	tasks[iTask].iDriver = iDriver;
	drivers[iDriver].available += map[drivers[iDriver].iLocation][tasks[iTask].iVenue];
	drivers[iDriver].available = calNextAvailable(drivers[iDriver].available, tasks[iTask].ready, linger);
	drivers[iDriver].iLocation = tasks[iTask].iVenue;
	if (tasks[iTask].iAsgnDriver == iDriver) {
		//assert the last taskAtHand is chosen 
		int j=drivers[iDriver].tasksAtHand.size() - 1;
		if (drivers[iDriver].tasksAtHand[j] != iTask) {
			fprintf(stderr, "internal error while assigning task to driver! poping task mismatch.\n");
			return;
		}
		drivers[iDriver].tasksAtHand.pop_back();
		tasks[iTask].iAsgnDriver = -1; //just for consistency: iDriver>=0 --> iAsgnDriver ==-1
	}
	if (tasks[iTask].iNextTask >= 0) {
		int j = tasks[iTask].iNextTask;
		if (tasks[j].iAsgnDriver == -1) {
			tasks[j].iAsgnDriver = iDriver;
			drivers[iDriver].tasksAtHand.push_back(j);
			arrange_future_tasks(drivers[iDriver], tasks);
		}
	}
	unsigned int n = drivers[iDriver].tasksAtHand.size();
	printf("At %.0lf, Driver %d finishes task %d, with %d upcoming tasks:", drivers[iDriver].available, iDriver, iTask, n);
	for (unsigned int i=0; i<n; i++)
		printf(" %d ", drivers[iDriver].tasksAtHand[n-1-i]);
	printf("\n");
}
int ALG::preProcess(CTime curTime, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, int &nLocations)
{
	vector<CLocationID> locationIDs;
	//set locations and paths
	for (unsigned int i=0; i<paths.size(); i++) {
		int findID = -1;
		for (unsigned int j=0; j<locationIDs.size(); j++)
			if (locationIDs[j] == paths[i].start){
				findID = j;
				break;
			}
		if (findID < 0) {
			locationIDs.push_back(paths[i].start);
			findID = locationIDs.size() -1;
		}
		paths[i].iSrc = findID;
		findID = -1;
		for (unsigned int j=0; j<locationIDs.size(); j++)
			if (locationIDs[j] == paths[i].end){
				findID = j;
				break;
			}
		if (findID < 0) {
			locationIDs.push_back(paths[i].end);
			findID = locationIDs.size() -1;
		}
		paths[i].iDst = findID;
	}
	nLocations = locationIDs.size();
	if (nLocations > MAXNLOCATIONS)
		return E_MAX_LOCATION; // too many locations
	for (int i=0; i<nLocations; i++)
		for (int j=0; j<nLocations; j++)
			map[i][j] = (i==j)? 0: -1;
	for (unsigned int i=0; i<paths.size(); i++) 
		map[paths[i].iSrc][paths[i].iDst] = paths[i].time;
	/* find the shortest paths, this should NOT be necessary */
	for (int k=0; k<nLocations; k++)
		for (int i=0; i<nLocations; i++) {
			if (i==k) continue;
			for (int j=0; j<nLocations; j++) {
				if (j==i || j==k) continue;
				if (map[i][k]>=0 && map[k][j]>=0 && (map[i][j] < 0 || map[i][j] > map[i][k]+map[k][j]))
					map[i][j] = map[i][k] + map[k][j];
			}
		}
	// set drivers, the tasklist maybe updated later according to curtask
	for (unsigned int i=0; i<drivers.size(); i++) {
		if (drivers[i].available <= curTime) {
			drivers[i].available = curTime;
			if (drivers[i].curtask != NULL_ID) 
				drivers[i].available += DELAYED_TASK_ESTIMATION_MS;
		}
		drivers[i].tasksAtHand.clear();
		drivers[i].taskList.clear();
		drivers[i].origin_available = drivers[i].available;
		drivers[i].iLocation = -1;
		for (int j=0; j<nLocations; j++) 
			if (drivers[i].location == locationIDs[j]) {
				drivers[i].iLocation = j;
				break;
			}
		if (drivers[i].iLocation == -1) return E_UNKNOWN_LOC_DRIVER;
		drivers[i].iOrigin_location = drivers[i].iLocation;
	}
	// convert tasks.asgnDriverID/prevTaskID to integer id -- the corresponding index
	// set drivers[].tasksAtHand tasks[].iNextTask
	for (unsigned int i=0; i<tasks.size(); i++) tasks[i].iNextTask = -1;
	for (unsigned int i=0; i<tasks.size(); i++) {
		tasks[i].iDriver = -1;
		for (unsigned int j=0; j<drivers.size(); j++) {
			if (drivers[j].curtask == tasks[i].tid) {
				tasks[i].iDriver = j;
				drivers[j].taskList.push_back(i);
				break;
			}
		}
		tasks[i].iAsgnDriver = -1;
		if (tasks[i].iDriver == -1 && tasks[i].did != NULL_ID) { //ignore tasks[i].did if it is someone's curtask
			for (unsigned int j=0; j<drivers.size(); j++)
				if (drivers[j].did == tasks[i].did) {
					drivers[j].tasksAtHand.push_back(i);
					tasks[i].iAsgnDriver = j;
					break;
				} 
			if (tasks[i].iAsgnDriver == -1) return E_UNKNOWN_DRIVER_TASK;
		}
		tasks[i].iPrevTask = -1;
		if (tasks[i].depend != NULL_ID) {
			for (unsigned int j=0; j<tasks.size(); j++) 
				if (tasks[j].tid == tasks[i].depend) {
					tasks[i].iPrevTask = j;
					tasks[j].iNextTask = i;
					break;
				}
			if (tasks[i].iPrevTask == -1) return E_UNKNOWN_DEPEND_TASK;
		}
		tasks[i].iVenue = -1;
		for (int j=0; j<nLocations; j++) 
			if (tasks[i].location == locationIDs[j]) {
				tasks[i].iVenue = j;
				break;
			}
		if (tasks[i].iVenue == -1) return E_UNKNOWN_LOC_TASK;
	}

	for (unsigned int i=0; i<tasks.size(); i++)
		if (tasks[i].iDriver >=0) {
			int j = tasks[i].iDriver;
			//check for consistency, not necessary 
			if (tasks[i].iVenue != drivers[j].iLocation)
				fprintf(stderr, "driver's available location and tasks's location mismatch.\n");
			if (tasks[i].iNextTask >=0 && tasks[tasks[i].iNextTask].iAsgnDriver != j)
				fprintf(stderr, "delivery task not properly pre-assigned.\n");
		}

	for (unsigned int i=0; i<drivers.size(); i++) {
		if (drivers[i].curtask != NULL_ID && drivers[i].taskList.size() != 1)
			return E_DUPLICATE_CURTASK; 
	}


	for (unsigned int i=0; i<drivers.size(); i++) 
		arrange_future_tasks(drivers[i], tasks); //after the arrangement, the tasks are in reverse order

	return E_NORMAL;
}

void ALG::_recursiveSearch(int driverIdx, vector<CDriver> & drivers, vector<CTask> & tasks, int &bestN, double &bestFTime, int bestSol[])
{
	CDriver & driver = drivers[driverIdx];
	CTime curTime = driver.available;
	int curLoc = driver.iLocation;
	int nFinished = driver.taskList.size();
	int n = tasks.size();
	if (nFinished >= bestN) {
		if (nFinished > bestN || curTime < bestFTime) {
			bestN = nFinished; bestFTime = curTime;
			for (int i=0; i<nFinished; i++)
				bestSol[i] = driver.taskList[i];
		}
	}
	if (n == bestN && curTime > bestFTime)
		return;
	for (int i=0; i<n; i++) if (tasks[i].iDriver<0) {
		CTime arvTime = curTime + map[curLoc][tasks[i].iVenue];
		if (tasks[i].iPrevTask >= 0 && tasks[tasks[i].iPrevTask].iDriver < 0) 
			continue; //its depend task hasn't finished
		if (arvTime <= tasks[i].deadline && arvTime <= driver.off) {
			driver.taskList.push_back(i);
			driver.available = (arvTime >= tasks[i].ready) ?  arvTime : tasks[i].ready;
			driver.iLocation = tasks[i].iVenue;
			tasks[i].iDriver = driverIdx;
			_recursiveSearch(driverIdx, drivers, tasks, bestN, bestFTime, bestSol);
			tasks[i].iDriver = -1;
			driver.taskList.pop_back();
		}
	}
}
int ALG::findScheduleSingleDriverOptimal(CTime curTime, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, vector<CScheduleItem> &schedule)
{
	int nLocations;
	int ret = preProcess(0, drivers, tasks, paths, nLocations); 
	if (ret != E_NORMAL)
		return ret;
	double bestFTime = 0;
	int bestN = 0;
	int bestSol[MAXNLOCATIONS]; //should be max task.size()
	_recursiveSearch(0, drivers, tasks, bestN, bestFTime, bestSol);
	/*
	for (int i=0; i<nLocations; i++) {
		for (int j=0; j<nLocations; j++) 
			printf("\t%.0f", map[i][j]);
		printf("\n");
	}
	printf("Alg: best duration: %.2f\n", bestFTime);
	*/
	schedule.clear(); 
	{
		int i=0;
		CScheduleItem si;
		si.did = drivers[i].did;
		si.tids.clear();
		for (unsigned int j=0; j<tasks.size(); j++) {
			int idx = bestSol[j];
			si.tids.push_back(tasks[idx].tid);
		}
		si.location = drivers[i].location;
		si.available = drivers[i].origin_available;
		si.updated = 0;
		schedule.push_back(si);
	}
	return E_NORMAL;
}

void resetPermutation(int * a, int n)
{
	for (int i=0; i<n; i++) a[i] = i;
}
bool nextPermutation(int * a, int n)
{
	int i=n-2;
	while (i>=0 && a[i]>a[i+1]) i--;
	if (i<0) return false;
	for (int j=i+1; j<n; j++) if (j+1>=n || a[j+1]<a[i]) {
		int temp = a[j];
		a[j] = a[i];
		a[i] = temp;
		break;
	}
	i++; 
	int j=n-1;
	while (i<j) {
		int temp = a[i];
		a[i] = a[j];
		a[j] = temp;
		i++; j--;
	}
	return true;
}
