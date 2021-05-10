#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <random>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <array>
#include <queue>
#include <algorithm>
#include <string>
#include <chrono>
#include <mutex>
#include <atomic>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <DirectXMath.h>
#include <cmath>

#include "SocketAddress.h"
#include "TCPSocket.h"
#include "SocketUtil.h"

//#include "SMathHelper.h"
#include "../../demo_server/demo_server/protocol.h"

#pragma comment(lib, "Ws2_32.lib")
class SMathHelper;