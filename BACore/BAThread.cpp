#include "BAThread.h"

void BAThread::Loop()
{
	ProcessMsg();
	Process();
}
