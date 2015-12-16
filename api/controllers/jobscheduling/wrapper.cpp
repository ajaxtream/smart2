#include <node.h>
#include <stdio.h>
#include <limits>
#include "json/json.h"
#include "jobSchedule.h"
using namespace v8;

bool parseInput(const char * cstr, int & algOpt, CTime & curTime, CRTime &deliLimit, vector<CDriver> & drivers, vector<CTask> & tasks, vector<CPath> & paths)
{
	drivers.clear(); tasks.clear(); paths.clear(); 
	Json::Reader reader;
	Json::Value root;
	std::string str(cstr);
	if (!reader.parse(str, root, false)) {
		//cout<< "parse failed:" << reader.getFormattedErrorMessages() << endl;
		printf("parse failed.\n"); 
		return false;
	}
	algOpt = root["algOpt"].asInt();
	curTime = root["curTime"].asDouble(); //asDouble returns 0.0 if it is null
	deliLimit = root["default_task_time"].asDouble();
	Json::Value driversA = root["drivers"];
	Json::Value tasksA = root["tasks"];
	Json::Value pathsA = root["paths"];
	for (unsigned int i=0; i<driversA.size(); i++) {
		CID driverID = driversA[i]["did"].asCString();
		CTime avlbTime = driversA[i]["available"].asDouble();
		CTime offWorkTime = driversA[i]["off"].asDouble();
		if (offWorkTime <= 0) offWorkTime = std::numeric_limits<double>::max(); //zero value means no off time
		CLocationID avlbLoc = driversA[i]["location"].asCString();
		CID curTask = driversA[i]["curtask"].isNull() ? NULL_ID : driversA[i]["curtask"].asCString(); 
		drivers.push_back(CDriver(driverID, avlbTime, offWorkTime, avlbLoc, curTask));
	}
	for (unsigned int i=0; i<tasksA.size(); i++) {
		CID id = tasksA[i]["tid"].asCString();
		CLocationID venue = tasksA[i]["location"].asCString();
		CTime deadline = tasksA[i]["deadline"].asDouble(); 
		if (deadline <= 0) deadline = std::numeric_limits<double>::max(); //zero value means no deadline
		CTime readyTime = tasksA[i]["ready_time"].asDouble();  
		CID asgnDriverID = tasksA[i]["did"].isNull() ? NULL_ID : tasksA[i]["did"].asCString(); 
		CID prevTaskID = tasksA[i]["depend"].isNull() ? NULL_ID : tasksA[i]["depend"].asCString(); 
		tasks.push_back(CTask(id,venue,deadline,readyTime,asgnDriverID,prevTaskID));
	}
	for (unsigned int i=0; i<pathsA.size(); i++) {
		CLocationID src = pathsA[i]["start"].asCString();
		CLocationID dst = pathsA[i]["end"].asCString();
		CRTime distance = pathsA[i]["time"].asDouble();
		paths.push_back(CPath(src, dst, distance));
	}
	/*
	printf("read %lu drivers, %lu tasks, %lu paths:\n", drivers.size(), tasks.size(), paths.size());
	for (unsigned int i=0; i<drivers.size(); i++) {
		printf("driver %d:  ID=%s, offTime = %.2f\n", i, drivers[i].did.c_str(), drivers[i].off);
	}
	for (unsigned int i=0; i<tasks.size(); i++) {
		printf("task %d:  ID=%s, deadline = %.2f\n", i, tasks[i].tid.c_str(), tasks[i].deadline);
	}
	*/
	return true;
}

std::string prepareOutput(vector<CScheduleItem> &schedule) 
{
	Json::Value root;
	Json::Value schd;
	Json::Value schdItem;
	for (unsigned int i=0; i<schedule.size(); i++) //if (schedule[i].asgnTaskID.size() > 0) {
	{
		schdItem["did"] = schedule[i].did;
		Json::Value taskList;
		for (unsigned int j=0; j<schedule[i].tids.size(); j++) {
			//Json::Value task;
			//task["taskID"] = schedule[i].asgnTaskID[j];
			//taskList.append(task);
			taskList.append(schedule[i].tids[j]);
		}
		schdItem["tids"] = taskList;
		schdItem["available"] = schedule[i].available;
		schdItem["off"] = schedule[i].off;
		schdItem["location"] = schedule[i].location;
		schdItem["updated"] = schedule[i].updated;
		schd.append(schdItem);
	}
	root["schedules"] = schd;

	Json::FastWriter writer;
	return writer.write(root);
}
std::string prepareErrOutput(int err_code, const char * customizedMsg) 
{
	Json::Value root;
	if (customizedMsg != NULL) root["err_msg"] = customizedMsg;
	else {
		switch (err_code) {
			case E_MAX_LOCATION:
				root["err_msg"] = "Too many locations.";
				break;
			case E_UNKNOWN_LOC_DRIVER:
				root["err_msg"] = "Cannot resolve driver's location in paths.";
				break;
			case E_UNKNOWN_LOC_TASK:
				root["err_msg"] = "Cannot resolve task's location in paths.";
				break;
			case E_UNKNOWN_DRIVER_TASK:
				root["err_msg"] = "Cannot resolve task's pre-assigned driver in driver list.";
				break;
			case E_UNKNOWN_DEPEND_TASK:
				root["err_msg"] = "Cannot resolve task's depend-task in task list.";
				break;
			case E_DUPLICATE_CURTASK:
				root["err_msg"] = "Duplicated current Task with the same task ID.";
				break;
			default:
				root["err_msg"] = "Unresolved Location ID or driver ID.";
		}
	}

	Json::FastWriter writer;
	return writer.write(root);
}

void Method(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	const char * cstr;
	String::Utf8Value str(args[0]->ToString());
	cstr = *str;

	int algOpt;
	CTime curTime;
	CRTime deliLimit;
	vector<CDriver> drivers;
	vector<CTask> tasks;
	vector<CPath> paths;
	vector<CScheduleItem> schedule;
	schedule.clear();
	int ret;
	Local<String> schd_string;
	if (parseInput(cstr, algOpt, curTime, deliLimit, drivers, tasks, paths)) {
		switch(algOpt) {
			case 0: 
				ret = ALG::findScheduleGreedy(curTime, deliLimit, drivers, tasks, paths, schedule);
				break;
			case 1: 
			default:
				ret = ALG::findScheduleSingleDriverOptimal(curTime, drivers, tasks, paths, schedule);
				break;
		}
		if (ret == E_NORMAL) 
			schd_string = String::NewFromUtf8(isolate, prepareOutput(schedule).c_str());
		else 
			schd_string = String::NewFromUtf8(isolate, prepareErrOutput(ret, NULL).c_str());
	}
	else
		schd_string = String::NewFromUtf8(isolate, prepareErrOutput(0, "parse input failed.").c_str());
	//args.GetReturnValue().Set(schd_string);
	Local<Function> cb = Local<Function>::Cast(args[1]);
	const unsigned int argc = 1;
	Local<Value> argv[argc] = {schd_string};
	cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
}

void init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "search", Method);
}

NODE_MODULE(jobSchedule, init)
