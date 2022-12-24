// Test.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "stdafx.h"
//#include <iostream>
//#include <queue>
//#include "BAMemoryPool.h"
//#include "BAScheduler.h"
//
//void ClientWorking()
//{
//	for (int i = 0; i < 10; i++)
//	{
//		SOCKET client;
//
//		client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//		SOCKADDR_IN server_addr;
//		memset(&server_addr, 0x00, sizeof(server_addr));
//		server_addr.sin_family = AF_INET;
//		server_addr.sin_port = htons(9999);
//		inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
//
//		int result = WSAConnect(client, (sockaddr*)&server_addr, sizeof(server_addr), nullptr, nullptr, nullptr, nullptr);
//		if (result == SOCKET_ERROR)
//		{
//			ErrorLog("client Error");
//		}
//
//		const char* sendbuf = "testtestetset";
//		send(client, sendbuf, strlen(sendbuf) + 1, 0);
//
//		Sleep(1000);
//
//		send(client, sendbuf, strlen(sendbuf) + 1, 0);
//
//		char buf[1000];
//		recv(client, buf, 1000, 0);
//
//		std::cout << buf << std::endl;
//	}
//}
////class JOB;
////BAScheduler<std::shared_ptr<JOB>> sch;
////
////class JOB 
////{
////private:
////	int id;
////	int calc;
////
////public:
////	JOB(int id) :id(id), calc(id) {}
////
////	void Process()
////	{
////		calc += 1;
////	}
////
////	void Complete()
////	{
////		InfoLog("ID[%d] NUM[%d]", id, calc );
////		switch (id)
////		{
////		case 1:
////		case 2:
////			sch.RegistJob(5000, std::shared_ptr<JOB>(this));
////			break;
////		case 3:
////		case 4:
////			sch.RegistJob(2000, std::shared_ptr<JOB>(this));
////			break;
////		}
////	}
////};
//
//int main()
//{
//	BANetworkEngine network;
//
//	network.Initialize();
//	network.StartNetwork();
//	
//	Sleep(1000);
//	std::thread t(ClientWorking);
//
//	t.join();
//
//	//
//	//sch.RegistJob(10000, std::make_shared<JOB>(1));
//	//sch.RegistJob(1000, std::make_shared<JOB>(2));
//	//sch.RegistJob(8000, std::make_shared<JOB>(3));
//	//sch.RegistJob(10000, std::make_shared<JOB>(4));
//
//	//while (1)
//	//{
//	//	sch.Loop();
//	//}
//}