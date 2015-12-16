#include <node.h>
#include <stdio.h>
#include "json/json.h"
using namespace v8;

void parseInput(const char * cstr)
{
	Json::Reader reader;
	Json::Value root;
	std::string str(cstr);
	if (!reader.parse(str, root, false)) {
		//cout<< "parse failed:" << reader.getFormattedErrorMessages() << endl;
		printf("parse failed."); 
		return;
	}
	Json::Value drivers = root["drivers"];
	Json::Value tasks = root["tasks"];
	Json::Value paths = root["paths"];
	printf("read %d drivers, %d tasks, %d paths:\n", drivers.size(), tasks.size(), paths.size());
	for (unsigned int i=0; i<drivers.size(); i++) {
		printf("driver %d:  ID=%s, offTime = %d\n", i, drivers[i]["ID"].asString().c_str(), drivers[i]["offTime"].asInt());
	}
}

std::string prepareOutput()
{
	Json::Value root;
	Json::Value schd;
	Json::Value schdItem;
	schdItem["driver"] = "driver1";
	schdItem["task"] = "task1";
	schd.append(schdItem);
	schdItem["driver"] = "driver2";
	schdItem["task"] = "task2";
	schd.append(schdItem);
	root["schedules"] = schd;

	Json::FastWriter writer;
	return writer.write(root);
}

void Method(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	const char * cstr;
	String::Utf8Value str(args[0]->ToString());
	cstr = *str;
	parseInput(cstr);
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, prepareOutput().c_str()));
}

void init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "hello", Method);
}

NODE_MODULE(addon, init)
