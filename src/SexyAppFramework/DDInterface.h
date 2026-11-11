#pragma once

#include "Common.h"
#include "CritSect.h"
#include "NativeDisplay.h"
#include "Rect.h"
#include "Ratio.h"

#include <ddraw.h>

namespace Sexy
{
    class DDInterface : public NativeDisplay
    {
    public:
        // TODO: Implement
        LPDIRECTDRAW mDD;
        // TODO: Implement
    };
}
