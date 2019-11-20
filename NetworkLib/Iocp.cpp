#include "stdafx.h"
#include "Iocp.h"
#include "NetworkSession.h"
#include <vector>

DWORD WorkerThreadCallback(__in LPVOID arg)
{

}

CIocp::CIocp()
{
}


CIocp::~CIocp()
{
}

BOOL CIocp::Begin()
{
	mIocpHandle = NULL;

	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);

	mWorkerThreadCount = system_info.dwNumberOfProcessors * 2;

	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (!mIocpHandle)
		return FALSE;

	mStartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if (mStartupEventHandle == NULL)
	{
		End();

		return FALSE;
	}

	for (DWORD i = 0; i < mWorkerThreadCount; i++)
	{
		HANDLE workerThread = CreateThread(NULL, 0, ::WorkerThreadCallback, this, 0, NULL);
		mWorkerThreadVector.push_back(workerThread);
		WaitForSingleObject(mStartupEventHandle, INFINITE);
	}
	return TRUE;
}

BOOL CIocp::End()
{
	for (DWORD i = 0; i < mWorkerThreadCount; i++)
	{
		PostQueuedCompletionStatus(mIocpHandle, 0, 0, NULL);
	}

	for (DWORD i = 0; i < mWorkerThreadCount; i++)
	{
		WaitForSingleObject(mWorkerThreadVector[i], INFINITE);
		CloseHandle(mWorkerThreadVector[i]);
	}

	if (mIocpHandle)
		CloseHandle(mIocpHandle);

	mWorkerThreadVector.clear();

	if (mStartupEventHandle)
		CloseHandle(mStartupEventHandle);

	return TRUE;
}

BOOL CIocp::RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey)
{
	if (!socket || !completionKey)
		return FALSE;

	mIocpHandle = CreateIoCompletionPort((HANDLE)socket, mIocpHandle, completionKey, 0);

	if (!mIocpHandle)
		return FALSE;

	return TRUE;
}

VOID CIocp::WorkerThreadCallback(VOID)
{
	BOOL Successed = FALSE;
	DWORD NumberOfByteTransfered = 0;
	VOID *CompletionKey = NULL;
	OVERLAPPED *Overlapped = NULL;
	OVERLAPPED_EX *OverlappedEx = NULL;
	VOID *object = NULL;

	while (TRUE)
	{
		SetEvent(mStartupEventHandle);

		Successed = GetQueuedCompletionStatus(
			mIocpHandle,
			&NumberOfByteTransfered,
			(LPDWORD)CompletionKey,
			&Overlapped,
			INFINITE);

		if (!CompletionKey)
			return;

		OverlappedEx = (OVERLAPPED_EX*)Overlapped;

		object = OverlappedEx->object;

		if (!Successed || (Successed && !NumberOfByteTransfered))
		{
			if (OverlappedEx->IoType == IO_ACCEPT)
				OnIoConnected(Object);
			else
				OnIoDisconnected(Object);

			continue;
		}

		switch (OverlappedEx->IoType)
		{
		case IO_READ:
			OnIoRead(Object, NumberOfByteTransfered);
			break;
		case IO_WRITE:
			OnIoWrite(Object, NumberOfByteTransfered);
			break;
		}
	}

	return VOID();
}
