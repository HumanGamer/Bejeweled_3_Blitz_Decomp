#pragma once

#include "Common.h"
#include "Flags.h"
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
        WidgetList mWidgets;
        WidgetManager* mWidgetManager;
        WidgetContainer* mParent;
        bool mUpdateIteratorModified;
        WidgetList::iterator mUpdateIterator;
        uint mLastWMUpdateCount;
        int mUpdateCnt;
        bool mDirty;
        int mX;
        int mY;
        int mWidth;
        int mHeight;
        bool mHasAlpha;
        bool mClip;
        FlagsMod mWidgetFlagsMod;
        int mPriority;
        int mZOrder;

    public:
        Widget* GetWidgetAtHelper(int x, int y, int theFlags, bool* found, int* theWidgetX, int* theWidgetY);

    public:
        WidgetContainer();
        virtual ~WidgetContainer();

        virtual Rect GetRect();
        virtual bool Intersects(WidgetContainer* theWidget);

        virtual void AddWidget(Widget* theWidget);
        virtual void RemoveWidget(Widget* theWidget);

        virtual void MarkAllDirty();

        virtual Point GetAbsPos(); // relative to top level

        // TODO: Implement

        virtual void SysColorChangedAll();
	    virtual void SysColorChanged();
    };
}
