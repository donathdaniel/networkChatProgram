
#include <stdio.h>
#include <iostream>
#include "winsock2.h"
#include "ws2tcpip.h"
#include "SysThread.h"
#include "MyThread.h"
using namespace std;

MyThread::MyThread(SOCKET socket, vector<MyThread*>& threadList, CRITICAL_SECTION& cs, string name, int id, string groups) 
    : threadList(threadList), cs(cs), groups(groups), id(id), userName(name)
{
    this->MySock = socket;
}


//---------------------------------------Connect-----------------------------------------
void MyThread::Connect(string sender) {
    this->userName = sender;
    this->id = this->threadList.size();

    string connectedNames = "#";
    for (auto& thread : this->threadList) {
        connectedNames.append(thread->userName + "#");
    }

    for (auto& thread : this->threadList) {
        int iResult = send(thread->MySock, connectedNames.c_str(), connectedNames.length(), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
            closesocket(this->MySock);
        }
    }
}


//---------------------------------Send public and private-----------------------------------
void MyThread::SendPublic(string sender, string message) {
    string writeMessage = "**PUBLIC MESSAGE FROM** " + sender + " >> " + message;

    for (auto& thread : this->threadList) {
        int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
            closesocket(this->MySock);
        }
    }
}

void MyThread::SendPrivate(string sender, string receiver, string message) {
    string writeMessage = "**PRIVATE MESSAGE FROM** " + sender + " >> " + message;

    for (auto& thread : this->threadList) {
        if (thread->userName == receiver) {
            int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                closesocket(this->MySock);
            }
        }
    }
} 


//---------------------------------Create Group-----------------------------------
bool checkIsMember(string receiver, string groups) {
    string delimiter = "#";

    size_t pos = 0;
    string token;
    bool isGroupName = true;
    while ((pos = groups.find(delimiter)) != string::npos) {
        token = groups.substr(0, pos);
        groups.erase(0, pos + delimiter.length());

        if (!isGroupName) {
            if (receiver == token) return true;
        }

        isGroupName = false;
    }
    if (receiver == groups) return true;

    return false;
}


void MyThread::CreateGroup(string sender, string message) {
    string writeMessage = "**YOU CREATED GROUP** " + message + " >> ";
    groups = message + "#" + sender;

    int iResult = send(this->MySock, writeMessage.c_str(), writeMessage.length(), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
        closesocket(this->MySock);
    }
}

void MyThread::AddToGroup(string sender, string receiver) {
    if (checkIsMember(receiver, this->groups)) {
        return;
    }

    this->groups.append("#" + receiver);
    size_t pos = groups.find("#");
    string groupName = groups.substr(0, pos);
    string writeMessage = "**YOU ARE ADDED TO THE GROUP** " + groupName;

    for (auto& thread : this->threadList) {
        if (thread->userName == receiver) {
            thread->groups = this->groups;
            int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                closesocket(this->MySock);
            }
        }
    }
}

void MyThread::SendToGroup(string sender, string message) {

    size_t pos = groups.find("#");
    string groupName = groups.substr(0, pos);
    string writeMessage = "**GROUP MESSAGE FROM** " + sender + " >> " + groupName + " >> " + message;

    string delimiter = "#";
    pos = 0;
    string token;
    bool isGroupName = true;
    string groupHelper = this->groups;

    while ((pos = groupHelper.find(delimiter)) != string::npos) {
        token = groupHelper.substr(0, pos);
        groupHelper.erase(0, pos + delimiter.length());

        if (!isGroupName) {
            for (auto& thread : this->threadList) {
                if (thread->userName == token) {
                    int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
                    if (iResult == SOCKET_ERROR)
                    {
                        printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                        closesocket(this->MySock);
                    }
                }

            }
        }

        isGroupName = false;
    }

    for (auto& thread : this->threadList) {
        if (thread->userName == groupHelper) {
            int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                closesocket(this->MySock);
            }
        }
    }
}

void MyThread::RemoveFromGroup(string sender, string receiver) {
    if (!checkIsMember(receiver, this->groups)) {
        return;
    }

    //remove from the groups string
    string newGroup;
    string groupName;
    string delimiter = "#";

    size_t pos = 0;
    string token;
    bool isGroupName = true;
    while ((pos = groups.find(delimiter)) != string::npos) {
        token = groups.substr(0, pos);
        groups.erase(0, pos + delimiter.length());

        if (!isGroupName) {
            if (receiver != token) 
                newGroup.append(token + "#");
        }
        else
        {
            newGroup.append(token + "#");
            groupName = token;
        }

        isGroupName = false;
    }
    if (receiver != groups)
        newGroup.append(groups + "#");

    if (newGroup != "") {
        newGroup.pop_back();
    }
    this->groups = newGroup;

    string writeMessage = "**YOU ARE REMOVED FROM THE GROUP** " + groupName;

    for (auto& thread : this->threadList) {
        if (thread->userName == receiver) {
            thread->groups = "";
            int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                closesocket(this->MySock);
            }
        }
    }
}

void MyThread::DeleteGroup(string sender) {
    string writeMessage = "**DELETE GROUP** " + sender;

    string delimiter = "#";
    size_t pos = 0;
    string token;
    bool isGroupName = true;
    string groupHelper = this->groups;

    while ((pos = groupHelper.find(delimiter)) != string::npos) {
        token = groupHelper.substr(0, pos);
        groupHelper.erase(0, pos + delimiter.length());

        if (!isGroupName) {
            for (auto& thread : this->threadList) {
                if (thread->userName == token) {
                    thread->groups = "";
                    int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
                    if (iResult == SOCKET_ERROR)
                    {
                        printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                        closesocket(this->MySock);
                    }
                }

            }
        }

        isGroupName = false;
    }

    for (auto& thread : this->threadList) {
        if (thread->userName == groupHelper) {
            thread->groups = "";
            int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                closesocket(this->MySock);
            }
        }
    }
}

void MyThread::SendFile(string sender, string receiver, string message) {
    //ifstream in(message);
    //string contents((std::istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    string writeMessage = "**PRIVATE FILE MESSAGE FROM** " + sender + " >> " + message;

    for (auto& thread : this->threadList) {
        if (thread->userName == receiver) {
            int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                closesocket(this->MySock);
            }
        }
    }
}


void MyThread::SendImage(string sender, string receiver, string message) {

    string writeMessage = "**PRIVATE IMAGE MESSAGE FROM** " + sender;

    for (auto& thread : this->threadList) {
        if (thread->userName == receiver) {
            int iResult = send(thread->MySock, writeMessage.c_str(), writeMessage.length(), 0);
            if (iResult == SOCKET_ERROR)
            {
                printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
                closesocket(this->MySock);
            }
        }
    }
}


void MyThread::routing(ActionType actionType, string& sender, string& receiver, string& message) {
    switch (actionType)
    {
    case ActionType::CONNECT: MyThread::Connect(sender); break;
    case ActionType::SEND_PUBLIC: MyThread::SendPublic(sender,message); break;
    case ActionType::SEND_PRIVATE: MyThread::SendPrivate(sender,receiver,message); break;
    case ActionType::CREATE_GROUP: MyThread::CreateGroup(sender, message); break;
    case ActionType::ADD_TO_GROUP: MyThread::AddToGroup(sender,receiver); break;
    case ActionType::SEND_TO_GROUP: MyThread::SendToGroup(sender,message); break;
    case ActionType::REMOVE_FROM_GROUP: MyThread::RemoveFromGroup(sender,receiver); break;
    case ActionType::DELETE_GROUP: MyThread::DeleteGroup(sender); break;
    case ActionType::SEND_FILE: MyThread::SendFile(sender,receiver,message); break;
    case ActionType::SEND_IMAGE: MyThread::SendImage(sender,receiver,message); break;
    default:
        break;
    }
}

string GetActionType(ActionType actionType) {
    string result;
    switch (actionType)
    {
    case ActionType::CONNECT: result = "CONNECT"; break;
    case ActionType::SEND_PUBLIC: result = "SEND_PUBLIC"; break;
    case ActionType::SEND_PRIVATE: result = "SEND_PRIVATE"; break;
    case ActionType::CREATE_GROUP: result = "CREATE_GROUP"; break;
    case ActionType::ADD_TO_GROUP: result = "ADD_TO_GROUP"; break;
    case ActionType::SEND_TO_GROUP: result = "SEND_TO_GROUP"; break;
    case ActionType::REMOVE_FROM_GROUP: result = "REMOVE_FROM_GROUP"; break;
    case ActionType::DELETE_GROUP: result = "DELETE_GROUP"; break;
    case ActionType::SEND_FILE: result = "SEND_FILE"; break;
    case ActionType::SEND_IMAGE: result = "SEND_IMAGE"; break;
    default:
        break;
    }

    return result;
}

void printMessage(ActionType actionType, std::string& sender, std::string& receiver, std::string& message) {
    printf("Message:\n");
    cout << "actionType: " << GetActionType(actionType) << endl;
    cout << "sender: " << sender << endl;
    cout << "receiver: -" << receiver << "-" << endl;
    cout << "message: -" << message << "-" << endl << endl;
}



void MyThread::processingTheRequest(char* recvBuf, int bufLen) {

    ActionType actionType = ActionType::CONNECT;
    int actionTypeInt;
    string sender, receiver, message;

    char delimiter[2] = "#";
    int counter = 0;

    char* token = strtok(recvBuf, delimiter);
    actionTypeInt = atoi(token);

    while (token != NULL)
    {

        token = strtok(NULL, delimiter);
        counter++;
        
        switch (counter)
        {
        case 1: sender = token; break;
        case 2: if (isspace(token[0]))  receiver = ""; else receiver = token; break;
        case 3: if (isspace(token[0]))  message = ""; else  message = token; break;
        default:
            break;
        }
    }

    switch (actionTypeInt)
    {
    case 0:  actionType = ActionType::CONNECT; break;
    case 1:  actionType = ActionType::SEND_PUBLIC; break;
    case 2:  actionType = ActionType::SEND_PRIVATE; break;
    case 3:  actionType = ActionType::CREATE_GROUP; break;
    case 4:  actionType = ActionType::ADD_TO_GROUP; break;
    case 5:  actionType = ActionType::SEND_TO_GROUP; break;
    case 6:  actionType = ActionType::REMOVE_FROM_GROUP; break;
    case 7:  actionType = ActionType::DELETE_GROUP; break;
    case 8:  actionType = ActionType::SEND_FILE; break;
    case 9:  actionType = ActionType::SEND_IMAGE; break;
    default: break;
    }

    routing(actionType, sender, receiver, message);
    printMessage(actionType, sender, receiver, message);
}

void MyThread::run() {
    int iResult;
    const int BufLen = 1024;
    char RecvBuf[BufLen] = {};
    char SendBuf[BufLen] = {};
    
    while (1) {

        iResult = recv(this->MySock, RecvBuf, BufLen, 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("Hiba a fogadasnal a kovetkezo hibakoddal: %d\n", WSAGetLastError());
            closesocket(MySock);
            return;
        }

        //printf("%s", RecvBuf);
        processingTheRequest(RecvBuf, BufLen);
        
        //------------------------------Critical section----------------------------------
        EnterCriticalSection(&this->cs);
        for (auto it = this->threadList.begin(); it != this->threadList.end(); it++) {
            if ((*it)->isExited()) {
                delete* it;
                threadList.erase(it);
            }

        }
        LeaveCriticalSection(&this->cs);
    }

}