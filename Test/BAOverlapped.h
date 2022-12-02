#pragma once
//
//struct BAReadOverlapped:public OVERLAPPED, public BAMemoryPool<BAReadOverlapped>
//{
//	WSABUF _wsa_buf;
//	char _buf[MAX_PACKET_SIZE];
//};
//
//struct BAWriteOverlapped: public OVERLAPPED, public  BAMemoryPool<BAWriteOverlapped>
//{
//	WSABUF _wsa_buf;
//	char _buf[MAX_PACKET_SIZE];
//};
//
//struct BAAcceptOverlapped :public OVERLAPPED, public  BAMemoryPool<BAAcceptOverlapped>
//{
//	WSABUF _wsa_buf;
//	char _buf[MAX_PACKET_SIZE];
//};
//

enum class OverlappedType
{
	READ = 0,
	WRITE = 1,
	ACCEPT = 2,
	CLOSE = 3
};

//이게 버퍼를 가지면안된다...
struct BAOverlapped : public OVERLAPPED, public  BAMemoryPool<BAOverlapped>
{
	OverlappedType type;
	WSABUF _wsa_buf;
};