#include <iostream>
#include <sstream>
#include <fstream>
#include "json/json.h"
using namespace std;

int main()
{
	string str = "{ 'a':'test'}";
	ifstream ifs;
	ifs.open("test.txt");
	Json::Reader reader;
	Json::Value root;
	//cout<<"string is : "<< ifs <<endl;
	if (!reader.parse(str, root, false)) {
		cout<< "parse failed:" << reader.getFormattedErrorMessages() << endl;
		return -1;
	}
	Json::Value a_value = root["a"];
	cout<<a_value.asString()<<endl;
	return 0;
}
