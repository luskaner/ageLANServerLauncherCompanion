#ifndef PCH_H
#define PCH_H

#include "framework.h"
#include <shellapi.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>
#include <cwctype>
#include <windows.h>
#include <ws2tcpip.h>
#include <wincrypt.h>
#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif
#include <security.h>
#include <schannel.h>
#include <winhttp.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "winhttp.lib")

#include "detours/detours.h"

#endif
