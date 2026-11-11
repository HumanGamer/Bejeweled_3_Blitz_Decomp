#pragma once

#include "Common.h"
#include "WidgetContainer.h"

namespace Sexy
{
    class SexyAppBase;

    class WidgetManager : public WidgetContainer
    {
    public:
        // TODO: Implement

        Rect mMouseDestRect;
        Rect mMouseSourceRect;
        bool mMouseIn;
        int mLastMouseX;
        int mLastMouseY;
        int mDownButtons;
        int mActualDownButtons;
        int mLastInputUpdateCnt;

        bool mKeyDown[255];
        int mLastDownButtonId;

        int mWidgetFlags;

    public:
	    WidgetManager(SexyAppBase* theApplet);
	    virtual ~WidgetManager();

        void DoMouseUps();
    };
}
