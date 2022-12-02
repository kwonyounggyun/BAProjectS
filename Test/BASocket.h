#pragma once

class BASocket : ISocket
{
private:
	SOCKET _socket;
	BABufferContainer _read_buf;
	BABufferContainer _write_buf;
	
public:
	BASocket();
	~BASocket();
	const SOCKET& GetSocket() { return _socket; }

	// ISocket��(��) ���� ��ӵ�
	virtual void Accept() override;
	virtual void Read() override;
	virtual void Write(void* msg, int size) override;

	void Close();
	bool InitSocket();
};