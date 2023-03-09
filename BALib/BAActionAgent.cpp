#include "pch.h"
#include "BAActionAgent.h"
#include "BAActor.h"

bool BAActionAgent::CheckActionable(ActionType type)
{
	switch (_action->GetType())
	{
	case ActionType::eIDLE:
	case ActionType::eMove:
	case ActionType::eCancleAbleAction:
		return true;
	case ActionType::eJump:
		if (type == ActionType::eJumpAction)
			return true;
		break;
	case ActionType::eJumpAction:
	case ActionType::eRoll:
	case ActionType::eAction:
		return false;
	default:
		break;
	}

	return false;
}

void BAActionAgent::ChangeAction(std::shared_ptr<Action> action)
{
	if (false == CheckActionable(action->GetType()))
		return;
	
	_action = action;
}

void BAActionAgent::Update()
{
	_action->Excute(_owner);
}
