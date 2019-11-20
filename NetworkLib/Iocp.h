#pragma once

class CIocp
{
private:
	HANDLE mIocpHandle;
	DWORD mWorkerThreadCount;

	HANDLE mStartupEventHandle;

	std::vector<HANDLE> mWorkerThreadVector;
public:
	CIocp();
	~CIocp();

	BOOL Begin();
	BOOL End();
	BOOL RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey);
	VOID WorkerThreadCallback(VOID);
};