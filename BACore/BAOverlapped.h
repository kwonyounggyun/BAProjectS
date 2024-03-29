#pragma once

class BASocket;
class BANetworkEngine;

enum class Overlapped_type : __int32
{
	NONE = -1,
	ACCEPT = 0,
	PRE_CONNECT = 1,
	CONNECT = 2,
	SEND = 3,
	RECV = 4,
	CLOSE = 5,
};

class BAOverlapped : public OVERLAPPED
{
protected:
	DWORD _trans_byte;
	Overlapped_type _type;
	BANetworkEngine* _engine;
	BAWeakPtr<BASocket> _socket;


public:
	explicit BAOverlapped(BAWeakPtr<BASocket>& socket) : _socket(socket), _trans_byte(0), _engine(nullptr), _type(Overlapped_type::NONE)
	{
	}

	void SetTransByte(DWORD trans_byte) { _trans_byte = trans_byte; }
	void SetEngine(BANetworkEngine* engine) { _engine = engine; }
	BASharedPtr<BASocket> GetSocket() { return _socket.lock(); }
	Overlapped_type GetType() { return _type; }
};

class BAOverlapped_Recv : public BAOverlapped
{
public:
	explicit BAOverlapped_Recv(BAWeakPtr<BASocket>& socket) : BAOverlapped(socket) 
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
	explicit BAOverlapped_Send(BAWeakPtr<BASocket>& socket) : BAOverlapped(socket), _send_byte(0)
	{
		_type = Overlapped_type::SEND;
	}
	void CompleteIO();
};

class BAOverlapped_Accept : public BAOverlapped
{
private:
	BASharedPtr<BASocket> _client;

public:
	explicit BAOverlapped_Accept(BAWeakPtr<BASocket>& socket) : BAOverlapped(socket) 
	{
		_type = Overlapped_type::ACCEPT;
	}
	void CompleteIO();
	void SetClient(BASharedPtr<BASocket>& client) { _client = client; }
};

class BAOverlapped_PreConnect : public BAOverlapped
{
private:
	BASharedPtr<BASocket> _connect;
public:
	explicit BAOverlapped_PreConnect(BAWeakPtr<BASocket>& socket) : BAOverlapped(socket)
	{
		_type = Overlapped_type::PRE_CONNECT;
		_connect = socket.lock();
	}
	void CompleteIO();
};

class BAOverlapped_Connect : public BAOverlapped
{
private:
	BASharedPtr<BASocket> _connect;
public:
	explicit BAOverlapped_Connect(BAWeakPtr<BASocket>& socket) : BAOverlapped(socket) 
	{
		_type = Overlapped_type::CONNECT;
		_connect = socket.lock();
	}
	void CompleteIO();
};

class BAOverlapped_Close : public BAOverlapped
{
public:
	explicit BAOverlapped_Close(BAWeakPtr<BASocket>& socket) : BAOverlapped(socket)
	{
		_type = Overlapped_type::CLOSE;
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
		case Overlapped_type::PRE_CONNECT:
			BA_DELETE(static_cast<BAOverlapped_PreConnect*>(overlapped));
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
		case Overlapped_type::CLOSE:
			BA_DELETE(static_cast<BAOverlapped_Close*>(overlapped));
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
		case Overlapped_type::PRE_CONNECT:
		{
			BAOverlapped_PreConnect* over = static_cast<BAOverlapped_PreConnect*>(overlapped);
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
		case Overlapped_type::CLOSE:
		{
			BAOverlapped_Close* over = static_cast<BAOverlapped_Close*>(overlapped);
			over->CompleteIO();
			BA_DELETE(over);
			break;
		}
		}
	}
};