#pragma once

class BASocket : ISocket
{
private:
	SOCKET _socket;
public:
	// ISocket��(��) ���� ��ӵ�
	virtual void Accept() override;
	virtual void Read() override;
	virtual void Write(void* msg, int size) override;
};

