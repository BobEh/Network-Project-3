#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

class Server
{
public:
	Server();

	void Update();

private:
	void SetNonBlocking(SOCKET socket);
	void ReadData();

	void UpdatePlayers();
	void BroadcastUpdate();

	bool mIsRunning;

	FD_SET mReadSet;
	timeval mTv;
	SOCKET mAcceptSocket;
	SOCKET mListenSocket;
};