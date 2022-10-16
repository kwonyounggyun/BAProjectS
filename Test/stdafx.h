#pragma once

#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

#include <algorithm>

#define MAX_PACKET_SIZE 4096

#include "ISocket.h"
#include "BACircularQueue.h"
#include "BACriticalSection.h"
#include "BAMemoryPool.h"
#include "BAOverlapped.h"
#include "BASocket.h"