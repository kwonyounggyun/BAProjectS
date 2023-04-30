#pragma once
#include "BACore.h"
#include "BAAction.h"

/*
* �׼��� �ѹ��� �ϳ��� �����ϴ�.
*/
class Action;
class BAActor;
class BAActionAgent
{
private:
	BAActor* _owner;
	std::shared_ptr<Action> _action;

public:
	BAActionAgent() : _owner(nullptr) 
	{
		_action = std::make_shared<IdleAction>();
	}
	~BAActionAgent() {}

	void SetOwner(BAActor* actor) { _owner = actor; }

private:
	bool CheckActionable(ActionType type);
public:
	void Update();
};

