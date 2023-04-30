#include "pch.h"
#include "BAActor.h"

void BAActor::Update(__time64_t time)
{
	if (time > GetLastUpdateTime() + 300)
	{
		_action_agent.Update();

		SetLastUpdateTime(time);
	}
}

void BAActor::Move(const BVector3D& direction, const BVector3D& forward)
{
	BVector3D move;

	if (direction.DotProduct(forward) <= 4)
	{
		move = direction * (_speed * (float)0.8);
	}
	else
	{
		move = direction * _speed;
	}

	_pos = _pos + move;
	_forward = forward;
}
