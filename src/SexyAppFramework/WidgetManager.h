#pragma once

#include "Common.h"
#include "Flags.h"
#include "KeyCodes.h"
#include "WidgetContainer.h"
#include "Widget.h"

namespace Sexy
{
    class MemoryImage;
    class SexyAppBase;

    typedef std::list<Widget*> WidgetList;

    enum
    {
        WIDGETFLAGS_UPDATE			= 1,
        WIDGETFLAGS_MARK_DIRTY		= 2,
        WIDGETFLAGS_DRAW			= 4,
        WIDGETFLAGS_CLIP			= 8,
        WIDGETFLAGS_ALLOW_MOUSE		= 16,
        WIDGETFLAGS_ALLOW_FOCUS		= 32,
    };

    class PreModalInfo
    {
    public:
        Widget* mBaseModalWidget;
        Widget* mPrevBaseModalWidget;
        Widget* mPrevFocusWidget;
        FlagsMod mPrevBelowModalFlagsMod;
    };

    typedef std::list<PreModalInfo> PreModalInfoList;
    typedef std::vector<std::pair<Widget*, int> > DeferredOverlayVector;

    class WidgetManager : public WidgetContainer
    {
    public:
        Widget* mDefaultTab;
        Graphics* mCurG;
        SexyAppBase* mApp;
        MemoryImage* mImage;
        MemoryImage* mTransientImage;
        bool mLastHadTransients;
        Widget* mPopupCommandWidget;
        DeferredOverlayVector mDeferredOverlayWidgets;
        int mMinDeferredOverlayPriority;
        bool mHasFocus;
        Widget* mFocusWidget;
        Widget* mLastDownWidget;
        Widget* mOverWidget;
        Widget* mBaseModalWidget;
        FlagsMod mLostFocusFlagsMod;
        FlagsMod mBelowModalFlagsMod;
        FlagsMod mDefaultBelowModalFlagsMod;
        PreModalInfoList mPreModalInfoList;
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

    protected:
	    int GetWidgetFlags();
        void MouseEnter(Widget* theWidget);
        void MouseLeave(Widget* theWidget);

    public:
	    WidgetManager(SexyAppBase* theApp);
	    virtual ~WidgetManager();

	    void FreeResources();

        // TODO: Implement

        void Resize(const Rect& theMouseDestRect, const Rect& theMouseSourceRect);

        Widget* GetAnyWidgetAt(int x, int y, int* theWidgetX, int* theWidgetY);
        Widget* GetWidgetAt(int x, int y, int* theWidgetX, int* theWidgetY);
	    void SetFocus(Widget* aWidget);

        void DoMouseUps();

        bool DrawScreen();
        bool UpdateFrame();
        bool UpdateFrameF(float theFrac);
        void SetPopupCommandWidget(Widget* theList);
        void RemovePopupCommandWidget();
        void MousePosition(int x, int y);
        void RehupMouse();
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
