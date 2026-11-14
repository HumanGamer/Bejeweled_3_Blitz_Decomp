#pragma once

#include "Color.h"
#include "Common.h"
#include "WidgetContainer.h"

namespace Sexy
{
    class Widget : public WidgetContainer
    {
    public:
        bool mVisible;
        bool mMouseVisible;
        bool mDisabled;
        bool mHasFocus;
        bool mIsDown;
        bool mIsOver;
        bool mHasTransparencies;
        std::vector<Color> mColors;
        // TODO: Implement
    };
}
