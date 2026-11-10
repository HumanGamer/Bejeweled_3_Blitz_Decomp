#pragma once

#include "Common.h"

namespace Sexy
{
    // TODO: use std::recursive_mutex for cross platform
    // constructor = InitializeCriticalSection
    // deconstructor = DeleteCriticalSection
    // lock = EnterCriticalSection
    // unlock = LeaveCriticalSection

    class CritSect
    {
    private:
        CRITICAL_SECTION mCriticalSection;
        friend class AutoCrit;

    public:
        CritSect();
        ~CritSect();
    };
}
