#pragma once

struct BAReadOverlapped:public OVERLAPPED, public BAMemoryPool<BAReadOverlapped>
{
	WSABUF _wsa_buf;
	char _buf[MAX_PACKET_SIZE];
};

struct BAWriteOverlapped: public OVERLAPPED, public  BAMemoryPool<BAWriteOverlapped>
{
	WSABUF _wsa_buf;
	char _buf[MAX_PACKET_SIZE];
};

struct BAAcceptOverlapped :public OVERLAPPED, public  BAMemoryPool<BAAcceptOverlapped>
{
	WSABUF _wsa_buf;
	char _buf[MAX_PACKET_SIZE];
};