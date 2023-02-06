#pragma once
#include <map>
#include "BAThread.h"
#include "BANetworkEngine.h"


class BABaseFrame : public BANetworkEngine
{
private:
	std::map<std::shared_ptr<BASession>, ULONG_PTR> _sessions;
	std::map<std::shared_ptr <BAThread>, ULONG_PTR> _threads;

public:
	// BANetworkEngine을(를) 통해 상속됨
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) override;


};

