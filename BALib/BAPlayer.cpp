#include "pch.h"
#include "BAPlayer.h"
#include "BAPacket.h"
#include "NetPlayerHandler.h"

void BAPlayer::CallHandle(DWORD protocol, void* msg)
{
	NetPlayerHandler::Call(protocol, msg, this);
}

void BAPlayer::AddTask(std::shared_ptr<Task> task)
{
	((SerializedObject*)this)->AddTask(task);
}

void BAPlayer::Move(const BVector3D& direction, const BVector3D& forward)
{
	BAActor::Move(direction, forward);

	auto msg = NetMessage::CreateMsg();
	msg->SetProtocol(_reply_su_move);
	auto reply = msg->GetBuffer<reply_su_move>();
	reply->_pos = GetPos();
	msg->SetSize(reply->GetSize());

	//�̵� �������� Ȯ�� ��.. ��ε� ĳ��Ʈ �ؾ��ϴµ�
	_session->SendMsg(msg);
}
