#pragma once

#include <WinSock2.h>
#include <mswsock.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

#include <algorithm>
#include <tchar.h>
#include <vector>
#include <map>

#define MAX_PACKET_SIZE 4096

#include "ISocket.h"
#include "INetworkManager.h"
#include "Log.h"

#include "BACircularQueue.h"
#include "BACriticalSection.h"
#include "BAMemoryPool.h"
#include "BAOverlapped.h"
#include "BASocket.h"
#include "BANetworkManager.h"