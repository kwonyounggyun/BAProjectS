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
	BASharedPtr<Action> _action;

public:
	BAActionAgent() : _owner(nullptr) 
	{
		_action = BAMakeShared<IdleAction>();
	}
	~BAActionAgent() {}

	void SetOwner(BAActor* actor) { _owner = actor; }

private:
	bool CheckActionable(ActionType type);
public:
	void Update();
};

