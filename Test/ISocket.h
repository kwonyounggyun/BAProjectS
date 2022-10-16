#pragma once
#include <Windows.h>

class ISocket
{
public:
	virtual void Accept() = 0;
	virtual void Read() = 0;
	virtual void Write(void* msg, int size) = 0;
};