#include "Debug.h"

#include "AutoCrit.h"
#include "CritSect.h"

#include <time.h>

#include "memmgr.h"

extern bool gInAssert = false;
extern bool gSexyDumpLeakedMem = false;

static FILE *gTraceFile = nullptr;
static int gTraceFileLen = 0;
static int gTraceFileNum = 1;

using namespace Sexy;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct SEXY_ALLOC_INFO
{
    int size;
    char file[MAX_PATH + 1];
    int line;
};
static bool gShowLeaks = false;
static bool gSexyAllocMapValid = false;

void SexyTraceFmt(const SexyChar* fmt ...)
{
    // TODO: Implement
}

void OutputDebug(const SexyChar* fmt ...)
{
    va_list argList;
    va_start(argList, fmt);
    std::string result = SexyStringToStringFast(vformat(fmt, argList));
    va_end(argList);

    OutputDebugStringA(result.c_str());
}

void OutputDebugStrF(const char* fmt ...)
{
    va_list argList;
    va_start(argList, fmt);
    std::string result = vformat(fmt, argList);
    va_end(argList);

    OutputDebugStringA(result.c_str());
}

void OutputDebugStrF(const wchar_t* fmt ...)
{
    va_list argList;
    va_start(argList, fmt);
    std::wstring result = vformat(fmt, argList);
    va_end(argList);

    OutputDebugStringW(result.c_str());
}
