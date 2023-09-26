class INetworkBuffer
{
public:
	virtual bool GetBuffer(int size, void* buffer) = 0;
	/* OnRecv is operated when recv-event complete.*/
	virtual void OnRecv(int size) = 0;
	/* OnSend is operated when send-event complete.*/
	virtual void OnSend(int size) = 0;

private:
	virtual void ReadBuffer() = 0;
	virtual void WriteBuffer() = 0;
};

class NetworkBufferReader
{
private:
	INetworkBuffer* _buf;
public:
	NetworkBufferReader(INetworkBuffer* buf) : _buf(buf)
	{
	}

	void Read()
	{

	}
};


class NetworkBufferWriter
{
private:
	INetworkBuffer* _buf;
public:
	NetworkBufferWriter(INetworkBuffer* buf) : _buf(buf)
	{
	}
};