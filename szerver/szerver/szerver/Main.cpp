#include <stdio.h>
#include "winsock2.h"
#include "ws2tcpip.h"
#include"SysThread.h"
#pragma comment(lib, "ws2_32.lib")
#include"MyThread.h"
#include<iostream>

using namespace std;


struct user {
	string name;
	int number;
};

void main() {

	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return;
	}
	//----------------------
	// Create a SOCKET for listening for
	// incoming connection requests.
	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in service;
	int port = 27015;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	if (bind(ListenSocket,
		(SOCKADDR*)&service,
		sizeof(service)) == SOCKET_ERROR) {
		printf("bind() failed.\n");
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	//----------------------
	// Listen for incoming connection requests.
	// on the created socket
	if (listen(ListenSocket, 1) == SOCKET_ERROR) {
		printf("Error listening on socket.\n");
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	//----------------------
	// Create a SOCKET for accepting incoming requests.
	SOCKET AcceptSocket;
	vector<MyThread*> threadList;
	vector<user*>users;
	vector<MyThread*>::iterator it;
	//----------------------
	// Accept the connection.

	CRITICAL_SECTION cs;

	InitializeCriticalSection(&cs);

	while (1) {
		printf("Waiting for client to connect...\n");
		AcceptSocket = accept(ListenSocket, NULL, NULL);
		if (AcceptSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		else
			printf("Client connected.\n\n");
		//-----------------------------------------------
		// Call the recvfrom function to receive datagrams
		// on the bound socket.

		MyThread* Thread = new MyThread(AcceptSocket, threadList, cs, "", 0, "");
		threadList.push_back(Thread);
		Thread->start();

	}

	printf("Finished sending. Closing socket.\n");
	closesocket(AcceptSocket);
	//---------------------------------------------
	// Clean up and quit.

	printf("Exiting.\n");
	WSACleanup();
	return;
}
