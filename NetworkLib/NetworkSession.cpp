#include "stdafx.h"
#include "NetworkSession.h"

CNetworkSession::CNetworkSession()
{
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	ZeroMemory(&mReadOverlapped, sizeof(mReadOverlapped));
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));
	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;
	mReliableUdpThreadHandle = NULL;
	mReliableUdpThreadStartupEvent = NULL;
	mReliableUdpThreadDestroyEvent = NULL;
	mReliableUdpThreadWakeUpEvent = NULL;
	mReliableUdpWriteCompleteEvent = NULL;
	mIsReliableUdpSending = FALSE;

	mAcceptOverlapped.IoType = IO_ACCEPT;
	mReadOverlapped.IoType = IO_READ;
	mWriteOverlapped.IoType = IO_WRITE;

	mAcceptOverlapped.object = this;
	mReadOverlapped.object = this;
	mWriteOverlapped.object = this;
}


CNetworkSession::~CNetworkSession()
{
}

BOOL CNetworkSession::Begin(VOID)
{
	CThreadSync Sync;

	if (mSocket)
		return FALSE;

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;

	mReliableUdpThreadHandle = NULL;
	mReliableUdpThreadStartupEvent = NULL;
	mReliableUdpThreadDestroyEvent = NULL;
	mReliableUdpThreadWakeUpEvent = NULL;
	mReliableUdpWriteCompleteEvent = NULL;

	mIsReliableUdpSending = FALSE;
	return TRUE;
}

BOOL CNetworkSession::End(VOID)
{
	CThreadSync Sync;

	if (!mSocket)
		return FALSE;

	shutdown(mSocket, SD_BOTH);
	closesocket(mSocket);

	mSocket = NULL;

	if (mReliableUdpThreadHandle)
	{
		SetEvent(mReliableUdpThreadDestroyEvent);
		WaitForSingleObject(mReliableUdpThreadHandle, INFINITE);
		CloseHandle(mReliableUdpThreadHandle);
	}

	if (mReliableUdpThreadDestroyEvent)
		CloseHandle(mReliableUdpThreadDestroyEvent);
	
	if (mReliableUdpThreadStartupEvent)
		CloseHandle(mReliableUdpThreadStartupEvent);

	if (mReliableUdpThreadWakeUpEvent)
		CloseHandle(mReliableUdpThreadWakeUpEvent);

	if (mReliableUdpWriteCompleteEvent)
		CloseHandle(mReliableUdpWriteCompleteEvent);

	mReliableWriteQueue.End();

	return TRUE;
}

BOOL CNetworkSession::TcpBind(VOID)
{
	CThreadSync Sync;

	if (mSocket)
		return FALSE;

	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (mSocket == INVALID_SOCKET)
		return FALSE;

	return TRUE;
}

BOOL CNetworkSession::Listen(USHORT port, INT backlog)
{
	CThreadSync sync;
	
	if (port <= 0 || backlog <= 0)
		return FALSE;

	if (!mSocket)
		return FALSE;

	SOCKADDR_IN listenSocketInfo;

	listenSocketInfo.sin_family = AF_INET;
	listenSocketInfo.sin_port = htons(port);
	listenSocketInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(mSocket, (struct sockaddr*) &listenSocketInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		End();
		return FALSE;
	}

	if (listen(mSocket, backlog) == SOCKET_ERROR)
	{
		End();

		return FALSE;
	}

	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 0;

	if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(LINGER)))
	{
		End();

		return FALSE;
	}

	return TRUE;
}

BOOL CNetworkSession::Connect(LPSTR address, USHORT port)
{
	CThreadSync sync;

	if (!address || port <= 0)
		return FALSE;

	if (!mSocket)
		return FALSE;

	SOCKADDR_IN RemoteAddressInfo;

	RemoteAddressInfo.sin_family = AF_INET;
	RemoteAddressInfo.sin_port = htons(port);
	RemoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(address);

	if (WSAConnect(mSocket, (LPSOCKADDR)& RemoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}
	}
	return TRUE;
}

BOOL CNetworkSession::Accept(SOCKET listenSocket)
{
	CThreadSync sync;

	if (!listenSocket)
		return FALSE;

	if (mSocket)
		return FALSE;

	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (mSocket == INVALID_SOCKET)
	{
		End();

		return FALSE;
	}

	if (!AcceptEx(listenSocket, mSocket, mReadBuffer, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &mAcceptOverlapped.Overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();

			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNetworkSession::InitializeReadForIocp(VOID)
{
	CThreadSync sync;

	if (!mSocket)
		return FALSE;
	WSABUF wsaBuf;
	DWORD ReadBytes = 0;
	DWORD ReadFlag = 0;

	wsaBuf.buf = (CHAR*)mReadBuffer;
	wsaBuf.len = MAX_BUFF;

	INT ReturnValue = WSARecv(mSocket, &wsaBuf, 1, &ReadBytes, &ReadFlag, &mReadOverlapped.Overlapped, NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError(0 != WSAEWOULDBLOCK))
	{
		End();
		return FALSE;
	}

	return TRUE;
}

BOOL CNetworkSession::ReadForIocp(BYTE * data, DWORD & dataLength)
{
	CThreadSync sync;

	if (!mSocket)
		return FALSE;

	if (!data || dataLength <= 0)
		return FALSE;

	CopyMemory(data, mReadBuffer, dataLength);

	return TRUE;
}

BOOL CNetworkSession::ReadForEventSelect(BYTE * data, DWORD & dataLength)
{
	CThreadSync sync;

	if (!mSocket)
		return FALSE;

	if (!data)
		return FALSE;

	if (!mSocket)
		return FALSE;

	WSABUF wsabuf;
	DWORD ReadBytes = 0;
	DWORD ReadFlag = 0;

	wsabuf.buf = (CHAR*)mReadBuffer;
	wsabuf.len = MAX_BUFF;

	INT returnValue = WSARecv(mSocket, &wsabuf, 1, &ReadBytes, &ReadFlag, &mReadOverlapped.Overlapped, NULL);

	if (returnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return FALSE;
	}

	CopyMemory(data, mReadBuffer, ReadBytes);
	dataLength = ReadBytes;

	return TRUE;
}

BOOL CNetworkSession::write(BYTE * data, DWORD dataLength)
{		
	return 0;
}
