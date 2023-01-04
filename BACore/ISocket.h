#pragma once
#include <Windows.h>

class ISocket
{
protected:

public:
	virtual void Read(void* msg, size_t size) = 0;
	virtual void Write(PACKET_HEADER& header,  void* msg, size_t size) = 0;
};