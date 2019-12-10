#include "server.h"

#include <winsock.h>
#include <WS2tcpip.h>

#include <ctime>

class Player
{
public:
	unsigned short port;
	struct sockaddr_in si_other;
	float x;
	float z;
	bool forward, back, left, right;
};

unsigned int numPlayersConnected = 0;

std::vector<Player> playersVec;

const float UPDATES_PER_SEC = 5;
std::clock_t curr;
std::clock_t prev;
double elapsed_secs;

void _PrintWSAError(const char* file, int line)
{
	int WSAErrorCode = WSAGetLastError();
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "[WSAError:%d] %S\n", WSAErrorCode, s);
	LocalFree(s);
}

Server::Server() : mIsRunning(false), mListenSocket(INVALID_SOCKET), mAcceptSocket(INVALID_SOCKET)
{
	playersVec.resize(8);

	WSAData WSAData;
	int iResult;
	int Port = 5150;
	SOCKADDR_IN ReceiverAddr;

	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0)
	{
		PrintWSAError();
		return;
	}

	mListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mListenSocket == INVALID_SOCKET)
	{
		PrintWSAError();
		return;
	}

	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(Port);
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	iResult = bind(mListenSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	if (iResult == SOCKET_ERROR)
	{
		PrintWSAError();
		return;
	}

	SetNonBlocking(mListenSocket);

	printf("[SERVER] Receiving IP: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
	printf("[SERVER] Receiving Port: %d\n", htons(ReceiverAddr.sin_port));
	printf("[SERVER] Ready to receive a datagram...\n");

	mIsRunning = true;
	prev = std::clock();
}

void Server::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}
}

void Server::Update(void)
{
	if (!mIsRunning) return;

	// TODO: ReadData, SendData
	ReadData();

	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);

	if (elapsed_secs < (1.0f / UPDATES_PER_SEC)) return;
	prev = curr;

	UpdatePlayers();
	BroadcastUpdate();
}

void Server::UpdatePlayers(void)
{
	for (int i = 0; i < numPlayersConnected; i++) {
		if (playersVec[i].forward) playersVec[i].z += 10.0f * elapsed_secs;
		if (playersVec[i].back) playersVec[i].z -= 10.0f * elapsed_secs;
		if (playersVec[i].left) playersVec[i].x += 10.0f * elapsed_secs;
		if (playersVec[i].right) playersVec[i].x -= 10.0f * elapsed_secs;
	}
}

void Server::BroadcastUpdate(void)
{
	// create our data to send, then send the same data to all players
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	memcpy(&(buffer[0]), &numPlayersConnected, sizeof(unsigned int));

	for (int i = 0; i < numPlayersConnected; i++) {
		float x = playersVec[i].x;
		float z = playersVec[i].z;
		memcpy(&(buffer[i * 8 + 4]), &x, sizeof(float));
		memcpy(&(buffer[i * 8 + 8]), &z, sizeof(float));
	}

	int result = sendto(mListenSocket, buffer, 12, 0,
		(struct sockaddr*) & (playersVec[0].si_other), sizeof(playersVec[0].si_other));
}

Player* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
{
	// If a player with this port is already connected, return it
	for (int i = 0; i < playersVec.size(); i++) {
		if (playersVec[i].port == port) return &(playersVec[i]);
	}

	// Otherwise create a new player, and return that one!
	playersVec[numPlayersConnected].port = port;
	playersVec[numPlayersConnected].x = 0.0f;
	playersVec[numPlayersConnected].z = 0.0f;
	playersVec[numPlayersConnected].si_other = si_other;
	return &(playersVec[numPlayersConnected++]);
}

void Server::ReadData(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[512];

	int result = recvfrom(mListenSocket, buffer, 512, 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			// printf(".");		// Quick test
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', 512);
		return;
	}


	unsigned short port = si_other.sin_port;

	Player* player = GetPlayerByPort(port, si_other);

	player->forward = buffer[0] == 1;
	player->back = buffer[1] == 1;
	player->left = buffer[2] == 1;
	player->right = buffer[3] == 1;

	printf("%d : %hu received { %d %d %d %d }\n", mListenSocket,
		port, player->forward, player->back, player->left, player->right);

	// Send the data back to the client
	// result = sendto(mListenSocket, buffer, 1, 0, (struct sockaddr*) & si_other, sizeof(si_other));
}