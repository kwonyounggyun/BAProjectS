#pragma once

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <mswsock.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <thread>

#include <iostream>

#include <algorithm>
#include <tchar.h>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <atomic>
#include <queue>
#include <functional>

#include <sql.h>
#include <sqlext.h>
#include <sal.h>

typedef std::basic_string<TCHAR> tstring;
#define BASharedPtr std::shared_ptr
#define BAWeakPtr std::weak_ptr
#define BAMakeShared std::make_shared

#include "BALock.h"
#include "BAMemoryPool.h"
#include "BACircularQueue.h"
#include "BAScheduler.h"
#include "BASingleton.h"
#include "BAThread.h"

#include "Log.h"

#define MAX_PACKET_SIZE 4096

/*BABufferContainer 에서 사용*/
#define MAX_UNIT_BUFFER_SIZE (MAX_PACKET_SIZE * 10)
#define MAX_USER 4000
#define MAX_INIT_BUFFER_UNIT MAX_USER * 100
#include "BABufferContainer.h"

#define MAX_NETWORK_BUF_COUNT 5

#define BA_NEW new
#define BA_DELETE(a) delete a;
#define BA_SAFE_DELETE(a) delete a; a = nullptr;

using PACKET_HEADER = WORD;
#define HEADER_SIZE  sizeof(PACKET_HEADER)
#define PACKET_PADDING 100

#define PROTOCOL_HEADER(group, type) group << 8 || type

#define MAX_SAVE_NETWORK_MSG 4000