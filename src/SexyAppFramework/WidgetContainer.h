#pragma once

#include "Common.h"
#include "Rect.h"

namespace Sexy
{
    class Graphics;
    class Widget;
    class WidgetManager;

    typedef std::list<Widget*> WidgetList;

    class WidgetContainer
    {
    public:
        // TODO: Implement

        int mX;
        int mY;
        int mWidth;
        int mHeight;

        // TODO: Implement

        virtual void MarkAllDirty();
    };
}
