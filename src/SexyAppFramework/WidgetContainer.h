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

        uint mLastWMUpdateCount;
        int mUpdateCnt;
        bool mDirty;
        int mX;
        int mY;
        int mWidth;
        int mHeight;
        bool mHasAlpha;
        bool mClip;

        // TODO: Implement

        virtual void MarkAllDirty();

        // TODO: Implement

        virtual void SysColorChangedAll();
	    virtual void SysColorChanged();
    };
}
