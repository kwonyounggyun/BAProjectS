#pragma once
#include <Windows.h>
#include "IOverlapped.h"

class ISocket
{
	SOCKET _socket;

	void Read();
	void Write(IOverlapped* overlapped );
};