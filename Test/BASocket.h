#pragma once

class BASocket : ISocket
{
private:
	SOCKET _socket;
	BABufferContainer _recv_buf;
	BABufferContainer _send_buf;
	//char _recv_buf[4000];
	
public:
	BASocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

	void Recv();

	// ISocket��(��) ���� ��ӵ�
	virtual void Accept() override;
	virtual void Recv(std::shared_ptr<BABufferUnitNode>& node) override;
	virtual void Send(void* msg, int size) override;

	void Close();
	bool InitSocket();

	// ISocket��(��) ���� ��ӵ�
	virtual void Read() override;
	virtual void Write(void* msg, int size) override;
};