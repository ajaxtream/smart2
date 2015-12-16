#ifndef PARAS_H
#define PARAS_H

#include <string>
#include <vector>
using std::string;
using std::vector;
#define MAXNLOCATIONS  100
//delayed task +5 min
#define DELAYED_TASK_ESTIMATION_MS   300000


/*
   class CTime
   {
   public: 
   CTime(int minuteToNow):_tMin(minuteToNow){}
   private:
   int _tMin;
   };
 */

typedef double CTime;
typedef double CRTime;
typedef string CID;
#define NULL_ID    ""

typedef CID CLocationID;
class ALG;

class CDriver
{
public:
	CDriver(CID driverID=NULL_ID, CTime availableTime=0, CTime offWorkTime=0, CLocationID availableLoc=NULL_ID, CID currentTask=NULL_ID)
		:did(driverID), curtask(currentTask), available(availableTime), off(offWorkTime), location(availableLoc){};
	CID 	did;
	CID 	curtask;
	CTime	available; /******* work var, may change in algorithm  ********************/
	CTime	off; // when he gets off work
	CLocationID	location;
protected:
	vector<int>	tasksAtHand; //work array: tasks that have been done by this driver in the FUTURE
	vector<int>	taskList; //work array: tasks (in sequence) that have been assigned to this driver till NOW 
	CTime   origin_available;
	int 	iOrigin_location;
	int	iLocation; //work var: driver's current location
	CTime   finishTime;
	CTime	delayTime;
	friend class ALG;
};

class CTask
{
public:
	CTask(CID taskID=NULL_ID, CLocationID destVenue=NULL_ID, CTime _deadline=0, CTime _readyTime=0, CID asgnDriver=NULL_ID, CID prevTask=NULL_ID)
	       	:tid(taskID), location(destVenue), deadline(_deadline), ready(_readyTime), did(asgnDriver), depend(prevTask){};
	CID 	tid;
	CLocationID	location;
	CTime	deadline; 
	CTime	ready;  // set this to a positive value if the driver has to wait when he arrives at the restaurant
	CID	did; //if != NULL_DRIVERID, this task can only be assigned to this driver
	CID	depend; // this task can only start after prevTaskID, and be carried out by the same driver
	//double	profit, penalty; //reserved for adjusting algorithm's behavior
	/******* may be changed in algorithm:  iAsgnDriver ********************/
protected:
	int	iVenue;
	int	iAsgnDriver; //corresponds to did, >= 0 if it appears in driver's tasksAtHand
	int	iDriver; //>=0 iff this task appears in driver's taskList, i.e., its execution order has been determined
	int	iPrevTask;
	int	iNextTask;
	friend class ALG;
};

class CPath
{
public:
	CPath(CLocationID src=NULL_ID, CLocationID dst=NULL_ID, CTime travelTime=0)
	       	:start(src), end(dst), time(travelTime) {};
	CLocationID start; 
	CLocationID end;
	CRTime time;
protected:
	int iSrc, iDst;
	friend class ALG;
};

class CScheduleItem
{ 
public:
	CID did;
	vector<CID>  tids; //the tasks assigned to this driver, in chronological order
	int updated;
	CTime available;
	CTime off;
	CLocationID location;
};

//parameter: drivers[in], tasks[in], paths[in], schedule[out]. returns the error code
//the private member of input classes may be altered. 
//abnormal cases include: ...
#define E_NORMAL  0
#define E_MAX_LOCATION 1
#define E_UNKNOWN_LOC_DRIVER 2
#define E_UNKNOWN_LOC_TASK 3
#define E_UNKNOWN_DRIVER_TASK 4
#define E_UNKNOWN_DEPEND_TASK 5
#define E_DUPLICATE_CURTASK 6
class ALG{
public:
	static int findScheduleGreedy(CTime curTime, CRTime deliLimit, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, vector<CScheduleItem> &schedule);
	static int findScheduleSingleDriverOptimal(CTime curTime, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, vector<CScheduleItem> &schedule);
	static int findScheduleBasic(CTime curTime, CRTime deliLimit, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, vector<CScheduleItem> &schedule);
private:
	static int preProcess(CTime curTime, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths, int &nLocations);
	static void assignTaskToDriver(int iTask, int iDriver, vector<CDriver> & drivers, vector<CTask> & tasks);
	static bool select_next_task(CDriver & driver, int iDriver, vector<CTask> & tasks, int & iSelectedTask);
	static void arrange_future_tasks(CDriver & driver, vector<CTask> & tasks);
	static double tryAddTask(int iTask, int iDriver, CDriver & driver, vector<CTask> & tasks);
	//return how good to choose (the new task) iTask as the next task, 
	//return 0 if infeasible or the next job at hand is better chosen as the next task
	static void calDFTime(int iStart, int iEnd, CDriver & driver, vector<CTask> & tasks, int curLoc, CTime & dTime, CTime & fTime);
	static void _recursiveSearch(int driverIdx, vector<CDriver> & drivers, vector<CTask> & tasks, int &bestN, double &bestFTime, int bestSol[]);
	static double map[MAXNLOCATIONS][MAXNLOCATIONS];
	static CRTime linger;
};
#endif
