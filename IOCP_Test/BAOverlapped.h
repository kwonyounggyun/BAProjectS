#pragma once
#include "BASocket.h"

class BASocket;

using overlapped_callback = std::function<bool(BASocket*, int)>;

class BAOverlapped : public OVERLAPPED
{
public:
	WSABUF _wsa_buf;
	overlapped_callback callback;

	BAOverlapped() {};
	virtual ~BAOverlapped() = 0;
};

class ReadOVERLAPPED : public BAOverlapped
{
public:
	ReadOVERLAPPED() {};
	virtual ~ReadOVERLAPPED() {};
};

class WriteOVERLAPPED : public BAOverlapped
{
public:
	char buf[MAX_MESSAGE_BUFFER];
};

class AcceptOVERLAPPED : public BAOverlapped
{

};

class CloseOVERLAPPED : public BAOverlapped
{

};