
// windows xp
//#define _WIN32_WINNT 0x0501
// windows 7
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// For ConvertArguments in Utilities
#include <shellapi.h>