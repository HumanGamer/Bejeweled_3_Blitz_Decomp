#pragma once

#include "Common.h"
#include "CritSect.h"

namespace Sexy
{
    // TODO: use std::recursive_mutex for cross platform
    // constructor = InitializeCriticalSection
    // deconstructor = DeleteCriticalSection
    // lock = EnterCriticalSection
    // unlock = LeaveCriticalSection

    class AutoCrit
    {
        LPCRITICAL_SECTION mCritSec;

    public:
        AutoCrit(LPCRITICAL_SECTION theCritSec) : mCritSec(theCritSec)
        {
            EnterCriticalSection(mCritSec);
        }

        AutoCrit(const CritSect& theCritSect) : mCritSec((LPCRITICAL_SECTION) &theCritSect.mCriticalSection)
        {
            EnterCriticalSection(mCritSec);
        }

        ~AutoCrit()
        {
            LeaveCriticalSection(mCritSec);
        }
    };
}
