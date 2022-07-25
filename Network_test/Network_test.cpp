
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


int main()
{
	WSADATA wsa_data;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (0 != result)
	{
		std::cout << "[ERROR] winsock 초기화 실패" << std::endl;
		return false;
	}

	SOCKET client = socket(PF_INET,  SOCK_STREAM, IPPROTO_TCP );

	if (client == INVALID_SOCKET)
	{
		std::cout << "[ERROR] socket  실패" << std::endl;
		return false;
	}

	SOCKADDR_IN socket_addr;
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(8000);
	socket_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (SOCKET_ERROR == connect(client, (SOCKADDR*)&socket_addr, sizeof(SOCKADDR)))
	{
		std::cout << "[ERROR] connect  실패" << std::endl;
		return false;
	}

	int count = 0;
	const char* test = "testttt";

	char buf[100];
	while (count < 10)
	{
		if (send(client, test, strlen(test) + 1, 0) < 0)
		{
			std::cout << "[ERROR] send  실패" << std::endl;
			continue;
		}
		Sleep(1000);
		if (recv(client, buf, 100, 0) < 0)
		{
			std::cout << "[ERROR] recv  실패" << std::endl;
			continue;
		}

		std::cout << buf << std::endl;
		count++;
	}

	closesocket(client);
	::WSACleanup();

	return 0;
}
