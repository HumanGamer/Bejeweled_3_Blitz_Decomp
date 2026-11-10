#pragma once

#pragma warning(disable:4786)
#pragma warning(disable:4503)

#undef _WIN32_WINNT
#undef WIN32_LEAN_AND_MEAN

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#undef _UNICODE
#undef UNICODE

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <cstdlib>

#include <Windows.h>
#include <shellapi.h>
#include <mmsystem.h>

#ifdef _USE_WIDE_STRING

typedef std::wstring SexyString;
#define _S(x)        L ##x

#define sexystrncmp			wcsncmp
#define sexystrcmp			wcscmp
#define sexystricmp			wcsicmp
#define sexysscanf			swscanf
#define sexyatoi			_wtoi
#define sexystrcpy			wcscpy

#define SexyStringToStringFast(x)	WStringToString(x)
#define SexyStringToWStringFast(x)	(x)
#define StringToSexyStringFast(x)	StringToWString(x)
#define WStringToSexyStringFast(x)	(x)

#ifndef SEXYFRAMEWORK_NO_REDEFINE_WIN_API
// Redefine the functions and structs we need to be wide-string
#undef CreateWindowEx
#undef RegisterClass
#undef MessageBox
#undef ShellExecute
#undef GetTextExtentPoint32
#undef RegisterWindowMessage
#undef CreateMutex
#undef DrawTextEx
#undef TextOut

#define CreateWindowEx				CreateWindowExW
#define RegisterClass				RegisterClassW
#define WNDCLASS					WNDCLASSW
#define MessageBox					MessageBoxW
#define ShellExecute				ShellExecuteW
#define GetTextExtentPoint32		GetTextExtentPoint32W
#define RegisterWindowMessage		RegisterWindowMessageW
#define CreateMutex					CreateMutexW
#define DrawTextEx					DrawTextExW
#define TextOut						TextOutW
#endif

#else

typedef std::string			SexyString;
#define _S(x)				x


#define sexystrncmp			strncmp
#define sexystrcmp			strcmp
#define sexystricmp			stricmp
#define sexysscanf			sscanf
#define sexyatoi			atoi
#define sexystrcpy			strcpy

#define SexyStringToStringFast(x)	(x)
#define SexyStringToWStringFast(x)	StringToWString(x)
#define StringToSexyStringFast(x)	(x)
#define WStringToSexyStringFast(x)	WStringToString(x)

#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef __int64 int64;

typedef std::map<std::string, std::string>		DefinesMap;
typedef std::map<std::wstring, std::wstring>	WStringWStringMap;
typedef SexyString::value_type					SexyChar;
#define HAS_SEXYCHAR

namespace Sexy
{
    extern HINSTANCE gHInstance;

    std::string GetFileDir(const std::string& thePath, bool withSlash = false);
}
