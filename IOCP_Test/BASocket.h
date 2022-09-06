#pragma once

class BAPacket
{
private:
	__int16 header;

public:
	BAPacket() {};
	virtual ~BAPacket() = 0;
	virtual size_t size()
	{
		return sizeof(*this);
	}
};

class BASocket
{
private:
	SOCKET _socket;
	char _recv_buffer[MAX_MESSAGE_BUFFER];

public:

	bool CloseSocket();
	const SOCKET& GetSocket() const { return _socket; }
	bool SetSocket(const SOCKET& socket);

	bool RecvMsg();
	bool SendMsg(void* buf, size_t size);
};

