#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include <MSWSock.h>

#define MAX_MSG_BUFFER 8192
class IOverlapped
{
public:
	OVERLAPPED _overlap;
	WSABUF _was_buf;
	char buf[MAX_MSG_BUFFER];

	IOverlapped() {}
	virtual ~IOverlapped() = 0;
};

IOverlapped::~IOverlapped()
{

}