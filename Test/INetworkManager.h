#pragma once

class INetworkManager
{
protected:
	virtual bool Initialize() = 0;
	virtual void StartNetwork() = 0;
};