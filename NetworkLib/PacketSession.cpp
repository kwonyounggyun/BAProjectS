#include "stdafx.h"
#include "PacketSession.h"


CPacketSession::CPacketSession()
{
	ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));

	mRemainLength = 0;
	mCurrentPacketNumber = 0;
	mLastPacketNumber = 0;
}


CPacketSession::~CPacketSession()
{
}

BOOL CPacketSession::Begin()
{
	CThreadSync sync;

	ZeroMemory(mPacketBuffer, sizeof(mPacketBuffer));

	mRemainLength = 0;
	mCurrentPacketNumber = 0;
	mLastPacketNumber = 0;

	if (!writeQueue.Begin())
		return FALSE;

	return CNetworkSession::Begin();
}
