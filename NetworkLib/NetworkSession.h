#pragma once
#include "MultiThreadSync.h"
#include "CircularQueue2.h"

#define MAX_BUFF 1024

typedef enum IO_TYPE {IO_READ, IO_WRITE, IO_ACCEPT};

typedef struct _OVERLAPPED_EX
{
	OVERLAPPED Overlapped;
	IO_TYPE IoType;

	VOID *object;
} OVERLAPPED_EX;

class CNetworkSession:CMultiThreadSync<CNetworkSession>
{
public:
	CNetworkSession();
	~CNetworkSession();

private:
	BYTE mReadBuffer[MAX_BUFF];

	OVERLAPPED_EX mAcceptOverlapped;
	OVERLAPPED_EX mReadOverlapped;
	OVERLAPPED_EX mWriteOverlapped;

	SOCKET mSocket;

	HANDLE mReliableUdpThreadHandle;
	HANDLE mReliableUdpThreadStartupEvent;
	HANDLE mReliableUdpThreadDestroyEvent;
	HANDLE mReliableUdpThreadWakeUpEvent;
	HANDLE mReliableUdpWriteCompleteEvent;
	BOOL mIsReliableUdpSending;

	CCircularQueue2<SOCKET> mReliableWriteQueue;

public:
	BOOL Begin(VOID);
	BOOL End(VOID);
	BOOL TcpBind(VOID);
	BOOL Listen(USHORT port, INT backlog);
	BOOL Connect(LPSTR address, USHORT port);
	BOOL Accept(SOCKET listenSocket);
	BOOL InitializeReadForIocp(VOID);

	BOOL ReadForIocp(BYTE* data, DWORD &dataLength);
	BOOL ReadForEventSelect(BYTE *data, DWORD &dataLength);
	BOOL write(BYTE *data, DWORD dataLength);
};

