#include "pch.h"
#include "BAPlayer.h"
#include "ActorManager.h"

void BAPlayer::CallHandle(DWORD protocol, void* msg)
{
	GetPlayerHandler()->Call(protocol, msg, this);
}

void BAPlayer::AddNetworkTask(BASharedPtr<ITask> task)
{
	SerializedObject::AddTask(task);
}

void BAPlayer::Move(const BVector3D& direction, const BVector3D& forward)
{
	BAActor::Move(direction, forward);

	auto msg = NetMessage::CreateMsg();
	msg->SetProtocol(_reply_su_move);
	auto reply = msg->GetBuffer<reply_su_move>();
	reply->id = id;
	reply->_pos = GetPos();
	msg->SetSize(reply->GetSize());

	//이동 가능한지 확인 후.. 브로드 캐스트 해야하는데
	//_session->SendMsg(msg);
	GetActorManager()->BroadcastMsg(msg);
}
