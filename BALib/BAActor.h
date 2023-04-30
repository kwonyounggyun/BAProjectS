#pragma once
#include "BAType.h"
#include "SerializedObject.h"
#include "BAActionAgent.h"

class BAActionAgent;
class BAActor : public SerializedObject
{
protected:
	BAActionAgent _action_agent;
	BVector3D _pos;
	BVector3D _forward;
	float _speed;

protected:
	virtual void Update(__time64_t time);

public:
	BVector3D GetPos() const { return _pos; }

public:
	BAActor() : _speed(0) 
	{
		_action_agent.SetOwner(this);
	}
	virtual ~BAActor() {}

	virtual void Initailize() {};
public:
	//Action
	virtual void Move(const BVector3D& direction, const BVector3D& forward);

	BAActionAgent* GetActionAgent() { return &_action_agent; }
};

