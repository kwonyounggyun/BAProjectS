#include "pch.h"
#include "BAAction.h"
#include "BAActor.h"

void MoveAction::Excute(BAActor* actor)
{
	actor->Move(_move_direction, _forward);
}

void IdleAction::Excute(BAActor* actor)
{
}
