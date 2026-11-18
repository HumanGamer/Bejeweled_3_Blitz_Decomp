#pragma once

#include "Common.h"
#include "KeyCodes.h"
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

        bool UpdateFrame();
        bool UpdateFrameF(float theFrac);

        void RemapMouse(int& theX, int& theY);
        bool MouseUp(int x, int y, int theClickCount);
        bool MouseDown(int x, int y, int theClickCount);
        bool MouseMove(int x, int y);
        bool MouseDrag(int x, int y);
        bool MouseExit(int x, int y);
        void MouseWheel(int theDelta);
	    bool KeyChar(SexyChar theChar);
        bool KeyDown(KeyCode key);
	    bool KeyUp(KeyCode key);

        bool KeyMessageRaw(HWND hwnd, uint message, unsigned __int64 wparam, __int64 lparam);
    };
}
