#pragma once
#include "BAType.h"

//����� �������� �����϶� ������ ����
//������ ��ų�� ����.
enum class ActionType
{
	eIDLE = 0,
	eMove = 1, //�ȱ�, �޸���, ����
	eRoll = 2, //������
	eJump = 3,
	eJumpAction = 4,
	eAction = 5, //��ų ���, ����
	eCancleAbleAction = 6,
};

class BAActor;
class Action
{
private:
	ActionType _type;
public:
	Action(ActionType type) : _type(type) {}
	virtual ~Action() {}
	ActionType GetType() { return _type; }

	virtual void Excute(BAActor* actor) = 0;
};

class MoveAction : public Action
{
private:
	BVector3D _move_direction;
	BVector3D _forward;
public:
	MoveAction(BVector3D direct, BVector3D forward) : Action(ActionType::eMove), _move_direction(direct), _forward(forward) {}
	virtual ~MoveAction() {}

	virtual void Excute(BAActor* actor);
};

