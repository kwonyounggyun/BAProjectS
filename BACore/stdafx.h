#pragma once

#include <WinSock2.h>
#include <mswsock.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <thread>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#include <algorithm>
#include <tchar.h>
#include <vector>
#include <map>
#include <set>

#define MAX_PACKET_SIZE 4096

#include "NetMessage.h"
#include "ISocket.h"
#include "INetworkEngine.h"
#include "Log.h"

/*BABufferContainer 에서 사용*/
#define MAX_UNIT_BUFFER_SIZE MAX_PACKET_SIZE * 10
#define MAX_USER 4000
#define MAX_INIT_BUFFER_UNIT MAX_USER * 100

#include "BACircularQueue.h"
#include "BACriticalSection.h"
#include "BAMemoryPool.h"
#include "BABufferContainer.h"

#include "BAOverlapped.h"
#include "BASocket.h"
#include "BANetworkEngine.h"