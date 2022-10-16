#pragma once

class BASocket : ISocket
{
private:
	SOCKET _socket;
public:
	// ISocket을(를) 통해 상속됨
	virtual void Accept() override;
	virtual void Read() override;
	virtual void Write(void* msg, int size) override;
};

