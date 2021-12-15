#pragma once
#include<vector>
#include<string>
#include <fstream>
using namespace std;

enum class ActionType {
	CONNECT = 0,
	SEND_PUBLIC = 1,
	SEND_PRIVATE = 2,
	CREATE_GROUP = 3,
	ADD_TO_GROUP = 4,
	SEND_TO_GROUP = 5,
	REMOVE_FROM_GROUP = 6,
	DELETE_GROUP = 7,
	SEND_FILE = 8,
	SEND_IMAGE = 9
};


class MyThread :public SysThread
{
private:
	SOCKET MySock;
	vector<MyThread*>& threadList;
	CRITICAL_SECTION& cs;
	string userName = "";
	int id = 0;
	string groups = "";

public:
	MyThread(SOCKET, vector<MyThread*>&, CRITICAL_SECTION&, string, int, string);
	virtual void run();

	void routing(ActionType, string&, string&, string&);
	void processingTheRequest(char*, int);

	void Connect(string);
	void SendPublic(string, string);
	void SendPrivate(string, string, string);
	void CreateGroup(string, string);
	void AddToGroup(string, string);
	void SendToGroup(string, string);
	void RemoveFromGroup(string, string);
	void DeleteGroup(string);
	void SendFile(string, string, string);
	void SendImage(string, string, string);

public:
	SOCKET getSocket() {
		return this->MySock;
	}
};

