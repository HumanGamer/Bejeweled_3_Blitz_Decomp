#pragma once

#include "Common.h"
#include "SoundManager.h"

namespace Sexy
{
    class DSoundManager : public SoundManager
    {
        friend class DSoundInstance;
        friend class DSoundMusicInterface;

    public:
        DSoundManager(HWND theHWnd, bool haveFMod);
        virtual ~DSoundManager();

        // TODO: Implement
    };
}