#pragma once
#include "NetworkSession.h"
#include "CircularQueue.h"

class CPacketSession : CNetworkSession
{
public:
	CPacketSession();
	~CPacketSession();

private:

	BYTE mPacketBuffer[MAX_BUFF];
	DWORD mRemainLength;
	DWORD mCurrentPacketNumber;
	DWORD mLastPacketNumber;
	CCircularQueue<> writeQueue;

public:
	BOOL Begin();
};

