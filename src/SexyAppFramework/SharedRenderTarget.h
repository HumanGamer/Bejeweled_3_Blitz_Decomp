#pragma once

#include "Common.h"
#include <objbase.h>

namespace Sexy
{
    class DDImage;

    class SharedRenderTarget
    {
    public:
        class Pool
        {
            class Entry
            {
                DDImage* mImage;
                IUnknown* mScreenSurface;
                SharedRenderTarget* mLockOwner;
                std::string mLockDebugTag;
            };

            std::vector<Pool::Entry> mEntries;
        };
    };
}
