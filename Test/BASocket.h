#pragma once

class BASocket : ISocket
{
private:
	SOCKET _socket;
	BABufferContainer _read_buf;
	BABufferContainer _write_buf;
	char _recv_buf[4000];
	
public:
	BASocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

	void Recv();

	// ISocket을(를) 통해 상속됨
	virtual void Accept() override;
	virtual void Read() override;
	virtual void Write(void* msg, int size) override;

	void Close();
	bool InitSocket();
};