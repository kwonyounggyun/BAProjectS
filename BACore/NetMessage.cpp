#include "NetMessage.h"

void NetMessage::Encrypt()
{
}

void NetMessage::Decrypt()
{
}

std::shared_ptr<NetMessage> NetMessage::CreateMsg()
{
    return std::make_shared<NetMessage>();
}
