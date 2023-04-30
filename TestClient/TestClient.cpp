#include "TestClient.h"
#include "ActorManager.h"

bool TestClient::Initialize(std::vector<NetworkConfig>& configs)
{
    BANetworkEngine::Initialize(configs);
    NetClientHandler::GetInstance()->Init();
    return false;
}

void TestClient::OnAcceptComplete(std::shared_ptr<BASession>& session)
{
}

void TestClient::OnConnectComplete(std::shared_ptr<BASession>& session)
{
    _client_sessions = session;
    std::cout << "connect" << std::endl;

    _player = std::make_shared<TestObject>();
    _client_sessions->SetObject(_player);
    start.store(true);
}

void TestClient::Loop()
{
    while (!start.load());
    int i = 0;
    while (start.load())
    {
        if (_player != nullptr)
            _player->Work();

        auto msg = NetMessage::CreateMsg();
        msg->SetProtocol(_request_us_move);
        auto packet = msg->GetBuffer<request_us_move>();
        packet->_forward = BVector3D(1, 0, 0);
        packet->_move = BVector3D(0, 1, 0);
        msg->SetSize(packet->GetSize());
        SendMsg(msg);

        if (i++ == 10000)
            break;
    }
}

void TestClient::SendMsg(std::shared_ptr<NetMessage>& msg)
{
    _client_sessions->SendMsg(msg);
}

void TestObject::CallHandle(DWORD protocol, void* msg)
{
    NetClientHandler::GetInstance()->Call(protocol, msg, this);
}

void TestObject::AddNetworkTask(std::shared_ptr<ITask> task)
{
    task->Execute();
}

void NetClientHandler::Call(DWORD protocol, void* msg, void* obj)
{
    auto iter = _handle.find(protocol);
    if(iter != _handle.end())
        (iter->second)(msg, obj);
}

bool NetClientHandler::Reply_SU_Move(void* msg, void* obj)
{
    auto packet_msg = reinterpret_cast<reply_su_move*>(msg);
    InfoLog("%d : [%f %f %f]", packet_msg->id, packet_msg->_pos.x, packet_msg->_pos.y, packet_msg->_pos.z);
    return true;
}

void NetClientHandler::Init()
{
    _handle[_reply_su_move] = NetClientHandler::Reply_SU_Move;
}
