#pragma once
#include "BACore.h"

class BASocket;
class BANetworkEngine;

enum class Overlapped_type
{
	ACCEPT = 0,
	CONNECT = 1,
	SEND = 2,
	RECV = 3
};

class BAOverlapped : public OVERLAPPED
{
protected:
	__int32 _error_code;
	std::weak_ptr<BASocket> _socket;
	DWORD _trans_byte;
	BANetworkEngine* _engine;
	Overlapped_type _type;

public:
	explicit BAOverlapped(std::weak_ptr<BASocket>& socket) : _error_code(0), _socket(socket), _trans_byte(0), _engine(nullptr)
	{
	}

	void SetTransByte(DWORD trans_byte) { _trans_byte = trans_byte; }
	void SetEngine(BANetworkEngine* engine) { _engine = engine; }
	std::shared_ptr<BASocket> GetSocket() { return _socket.lock(); }
	Overlapped_type GetType() { return _type; }
};

class BAOverlapped_Recv : public BAOverlapped
{
public:
	explicit BAOverlapped_Recv(std::weak_ptr<BASocket>& socket) : BAOverlapped(socket) 
	{
		_type = Overlapped_type::RECV;
	}
	void CompleteIO();
};

class BAOverlapped_Send : public BAOverlapped
{
public:
	DWORD _send_byte;
public:
	explicit BAOverlapped_Send(std::weak_ptr<BASocket>& socket) : BAOverlapped(socket) 
	{
		_type = Overlapped_type::SEND;
	}
	void CompleteIO();
};

class BAOverlapped_Accept : public BAOverlapped
{
private:
	std::shared_ptr<BASocket> _client;

public:
	explicit BAOverlapped_Accept(std::weak_ptr<BASocket>& socket) : BAOverlapped(socket) 
	{
		_type = Overlapped_type::ACCEPT;
	}
	void CompleteIO();
	void SetClient(std::shared_ptr<BASocket>& client) { _client = client; }
};

class BAOverlapped_Connect : public BAOverlapped
{
private:
	std::shared_ptr<BASocket> _connect;
public:
	explicit BAOverlapped_Connect(std::weak_ptr<BASocket>& socket) : BAOverlapped(socket) 
	{
		_type = Overlapped_type::CONNECT;
		_connect = socket.lock();
	}
	void CompleteIO();
};

class OverlappedTaskSeparator
{
public:
	static void Delete(BAOverlapped* overlapped)
	{
		switch (overlapped->GetType())
		{
		case Overlapped_type::ACCEPT:
			BA_DELETE(static_cast<BAOverlapped_Accept*>(overlapped));
			break;
		case Overlapped_type::CONNECT:
			BA_DELETE(static_cast<BAOverlapped_Connect*>(overlapped));
			break;
		case Overlapped_type::SEND:
			BA_DELETE(static_cast<BAOverlapped_Send*>(overlapped));
			break;
		case Overlapped_type::RECV:
			BA_DELETE(static_cast<BAOverlapped_Recv*>(overlapped));
			break;
		}
	}

	static void Work(BAOverlapped* overlapped)
	{
		switch (overlapped->GetType())
		{
		case Overlapped_type::ACCEPT:
		{
			BAOverlapped_Accept* over = static_cast<BAOverlapped_Accept*>(overlapped);
			over->CompleteIO();
			BA_DELETE(over);
			break;
		}
		case Overlapped_type::CONNECT:
		{
			BAOverlapped_Connect* over = static_cast<BAOverlapped_Connect*>(overlapped);
			over->CompleteIO();
			BA_DELETE(over);
			break;
		}
		case Overlapped_type::SEND:
		{
			BAOverlapped_Send* over = static_cast<BAOverlapped_Send*>(overlapped);
			over->CompleteIO();
			BA_DELETE(over);
			break;
		}
		case Overlapped_type::RECV:
		{
			BAOverlapped_Recv* over = static_cast<BAOverlapped_Recv*>(overlapped);
			over->CompleteIO();
			BA_DELETE(over);
			break;
		}
		}
	}
};