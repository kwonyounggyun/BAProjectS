#pragma once
#include <Windows.h>

class ISocket
{
public:
	virtual void Accept() = 0;
	virtual void Recv(std::shared_ptr<BABufferUnitNode>& node) = 0;
	virtual void Send(void* msg, int size) = 0;

public:
	virtual void Read() = 0;
	virtual void Write(void* msg, int size) = 0;
};