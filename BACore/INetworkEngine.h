#pragma once

class INetworkEngine
{
protected:
	virtual bool Initialize() = 0;
	virtual bool StartNetwork() = 0;
};