#pragma once

#pragma warning(disable:4786)
#pragma warning(disable:4503)

#undef _WIN32_WINNT
#undef WIN32_LEAN_AND_MEAN

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
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
    const ulong SEXY_RAND_MAX = 0x7FFFFFFF;

    extern bool gDebug;
    extern HINSTANCE gHInstance;

    int Rand();
    int Rand(int range);
    float Rand(float range);
    void SRand(ulong theSeed);

    extern std::string	vformat(const char* fmt, va_list argPtr);
    extern std::wstring	vformat(const wchar_t* fmt, va_list argPtr);
    extern std::string	StrFormat(const char* fmt ...);
    extern std::wstring	StrFormat(const wchar_t* fmt ...);

    bool CheckFor98Mill();
    bool CheckForVista();
    std::string GetAppDataFolder();
    void SetAppDataFolder(const std::string& thePath);

    std::wstring		StringToWString(const std::string &theString);
    std::string			WStringToString(const std::wstring &theString);
    SexyString			StringToSexyString(const std::string& theString);
    SexyString			WStringToSexyString(const std::wstring& theString);
    std::string			SexyStringToString(const SexyString& theString);
    std::wstring		SexyStringToWString(const SexyString& theString);
    SexyString          ToSexyString(const std::string& theString);
    SexyString          ToSexyString(const std::wstring& theString);

    bool StringToInt(const std::string theString, int* theIntVal);
    bool StringToDouble(const std::string theString, double* theDoubleVal);
    bool StringToInt(const std::wstring theString, int* theIntVal);
    bool StringToDouble(const std::wstring theString, double* theDoubleVal);

    std::string GetFileDir(const std::string& thePath, bool withSlash = false);

    std::string RemoveTrailingSlash(const std::string& theDirectory);
}
