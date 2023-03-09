#pragma once
#include "BACore.h"
#include "BAAction.h"

/*
* 액션은 한번에 하나만 가능하다.
*/
class Action;
class BAActor;
class BAActionAgent
{
private:
	BAActor* _owner;
	std::shared_ptr<Action> _action;

public:
	BAActionAgent() : _owner(nullptr) {}
	~BAActionAgent() {}

	void SetOwner(BAActor* actor) { _owner = actor; }

private:
	bool CheckActionable(ActionType type);
public:
	void ChangeAction(std::shared_ptr<Action> action);
	void Update();
};

