#pragma once

#include "Common.h"
#include "WidgetContainer.h"

namespace Sexy
{
    class MemoryImage;
    class SexyAppBase;

    class WidgetManager : public WidgetContainer
    {
    public:
        // TODO: Implement

        SexyAppBase* mApp;
        MemoryImage* mImage;
        MemoryImage* mTransientImage;
        bool mLastHadTransients;

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

        // TODO: Implement

        void Resize(const Rect& theMouseDestRect, const Rect& theMouseSourceRect);

        void DoMouseUps();
    };
}
