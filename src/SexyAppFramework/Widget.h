#pragma once

#include "Color.h"
#include "Common.h"
#include "Insets.h"
#include "KeyCodes.h"
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
        Insets mMouseInsets;
        bool mDoFinger;
        bool mWantsFocus;
        Widget* mTabPrev;
        Widget* mTabNext;

    public:
        Widget();
        virtual ~Widget();

        // TODO: Implement

        virtual void ShowFinger(bool on);

        virtual bool WantsFocus();
        // TODO: Implement

        virtual void GotFocus();
        virtual void LostFocus();
        virtual void KeyChar(SexyChar theChar);
        virtual void KeyDown(KeyCode theKey);
        virtual void KeyUp(KeyCode theKey);
        virtual void MouseEnter();
        virtual void MouseLeave();
        virtual void MouseMove(int x, int y);
        virtual void MouseDown(int x, int y, int theClickCount);
        virtual void MouseDown(int x, int y, int theBtnNum, int theClickCount);
        virtual void MouseUp(int x, int y);
        virtual void MouseUp(int x, int y, int theClickCount);
        virtual void MouseUp(int x, int y, int theBtnNum, int theClickCount);
        virtual void MouseDrag(int x, int y);
        virtual void MouseWheel(int theDelta);
	    virtual bool IsPointVisible(int x, int y);

        // TODO: Implement

        virtual bool Contains(int theX, int theY);
        virtual Rect GetInsetRect();
    };
}
