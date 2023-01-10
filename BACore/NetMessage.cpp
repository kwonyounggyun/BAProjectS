#include "NetMessage.h"

std::shared_ptr<NetMessage> NetMessage::CreateMsg()
{
    return std::make_shared<NetMessage>();
}
