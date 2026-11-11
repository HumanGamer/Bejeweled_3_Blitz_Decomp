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
