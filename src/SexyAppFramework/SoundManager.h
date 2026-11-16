#pragma once

#include <Windows.h>
#include "Common.h"

namespace Sexy
{
    class SoundManager
    {
    public:
        SoundManager() {}
        virtual ~SoundManager() {}

        virtual void StopAllSounds() = 0;

        // TODO: Implement
    };
}
