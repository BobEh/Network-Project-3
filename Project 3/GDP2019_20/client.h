#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include <vector>
#include <string>

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

class Client
{
public:
	Client();

	void CreateSocket(std::string ip, int port);
	void Update();
	void Send(char* data, int numBytes);
	void SetPosition(int id, float& x, float& y);

private:
	void SetNonBlocking(SOCKET socket);
	void Recv();

	SOCKET mServerSocket;
	struct sockaddr_in si_other;
};