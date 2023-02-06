#include "BABaseFrame.h"

void BABaseFrame::OnAcceptComplete(std::shared_ptr<BASession>& session)
{
    _sessions.insert(std::make_pair(session, (ULONG_PTR)session.get()));
}