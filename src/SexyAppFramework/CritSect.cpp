#pragma warning( disable : 4786 )

#include "CritSect.h"
#include <Windows.h>

using namespace Sexy;

////////////////////////////////////////////////////////////////////////////////

CritSect::CritSect()
{
    InitializeCriticalSection(&mCriticalSection);
}

////////////////////////////////////////////////////////////////////////////////

CritSect::~CritSect()
{
    DeleteCriticalSection(&mCriticalSection);
}
