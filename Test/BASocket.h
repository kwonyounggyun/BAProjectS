#pragma once

class BASocket : ISocket
{
private:
	SOCKET _socket;
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