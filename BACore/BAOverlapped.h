#pragma once
#include "BACore.h"

class BASocket;
class BANetworkEngine;
class BAOverlapped : public OVERLAPPED
{
protected:
	__int32 _error_code;
	ULONG_PTR _object;
	DWORD _trans_byte;
	BANetworkEngine* _engine;

public:
	explicit BAOverlapped(ULONG_PTR obj) : OVERLAPPED(), _error_code(0), _object(obj), _trans_byte(0)
	{
	}

	virtual ~BAOverlapped() {};
	virtual void CompleteIO() = 0;

	void SetTransByte(DWORD trans_byte) { _trans_byte = trans_byte; }
	void SetEngine(BANetworkEngine* engine) { _engine = engine; }
};

class BAOverlapped_Recv : public BAOverlapped
{
public:
	explicit BAOverlapped_Recv(ULONG_PTR obj) : BAOverlapped(obj) {}
	// BAOverlapped을(를) 통해 상속됨
	virtual void CompleteIO() override;
};

class BAOverlapped_Send : public BAOverlapped
{
//public:
//	std::shared_ptr<NetMessage> _msg;
//	DWORD _send_byte;
	ULONG_PTR _msg_key;
public:
	explicit BAOverlapped_Send(ULONG_PTR obj) : BAOverlapped(obj) {}//, _send_byte(0), _msg(nullptr) {}
	// BAOverlapped을(를) 통해 상속됨
	virtual void CompleteIO() override;
	void SetMsgKey(ULONG_PTR key) { _msg_key = key; }
	//void SetMsg(std::shared_ptr<NetMessage>& msg) { _msg = msg; }
};

class BAOverlapped_Accept : public BAOverlapped
{
public:
	explicit BAOverlapped_Accept(ULONG_PTR obj) : BAOverlapped(obj) {}
	// BAOverlapped을(를) 통해 상속됨
	virtual void CompleteIO() override;
};