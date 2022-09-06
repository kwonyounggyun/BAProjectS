#pragma once
class BAServerConfig
{
private:
	int max_client;
	int _iocp_thread_count;
public:
	int GetMaxClient() { return max_client; }
	int GetIOCPThreadCount() { return _iocp_thread_count; }
};

