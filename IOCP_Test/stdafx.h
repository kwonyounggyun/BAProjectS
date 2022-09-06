#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <vector>
#include <map>
#include <tchar.h>
#include <thread>
#include <atomic>
#include <functional>

#define MAX_MESSAGE_BUFFER  8192
#define SERVER_PORT 8000

#include "SocketManager.h"

