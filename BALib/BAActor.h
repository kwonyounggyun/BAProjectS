#pragma once
#include "BAType.h"
#include "SerializedObject.h"
#include "BAActionAgent.h"

class BAActionAgent;
class BAActor : public SerializedObject
{
private:
	BAActionAgent _action_agent;
	BVector3D _pos;
	BVector3D _forward;
	float _speed;

protected:
	void Update();
	BVector3D GetPos() { return _pos; }

public:
	BAActor() : _speed(0) 
	{
		_action_agent.SetOwner(this);
	}
	virtual ~BAActor() {}

public:
	//Action
	virtual void Move(const BVector3D& direction, const BVector3D& forward);
};

