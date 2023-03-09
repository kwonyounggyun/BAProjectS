#include "pch.h"
#include "BAActor.h"

void BAActor::Update()
{
	_action_agent.Update();
}

void BAActor::Move(const BVector3D& direction, const BVector3D& forward)
{
	BVector3D move;

	if (direction.DotProduct(forward) <= 4)
	{
		move = direction * (_speed * 0.8);
	}
	else
	{
		move = direction * _speed;
	}

	_pos = _pos + move;
	_forward = forward;
}
