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

class BA_OVERLAPPED : OVERLAPPED
{
public:
	WSABUF data_buf;
};

class ReadOVERLAPPED : public BA_OVERLAPPED
{
public:
	DWORD recv_bytes;
	DWORD flags;
};

class WriteOVERLAPPED : public BA_OVERLAPPED
{
public:
	DWORD send_bytes;
};

class AcceptOVERLAPPED : public BA_OVERLAPPED
{

};

class CloseOVERLAPPED : public BA_OVERLAPPED
{

};

class BASocket
{
private:
	SOCKET socket;
	char message_buffer[MAX_MESSAGE_BUFFER];
	DWORD recv_bytes;

public:
	bool recv_msg();
	bool send_msg(BAPacket* packet);
};

