#include "WidgetContainer.h"

#include "Flags.h"
#include "WidgetManager.h"

using namespace Sexy;

WidgetContainer::WidgetContainer()
{
    // TODO: Implement
}

WidgetContainer::~WidgetContainer()
{
    // TODO: Implement
}

Rect WidgetContainer::GetRect()
{
    // TODO: Implement
    return {};
}

bool WidgetContainer::Intersects(WidgetContainer *theWidget)
{
    // TODO: Implement
    return false;
}

void WidgetContainer::AddWidget(Widget *theWidget)
{
    // TODO: Implement
}

void WidgetContainer::RemoveWidget(Widget *theWidget)
{
    // TODO: Implement
}

Widget *WidgetContainer::GetWidgetAtHelper(int x, int y, int theFlags, bool *found, int *theWidgetX, int *theWidgetY)
{
    bool belowModal = false;

    ModFlags(theFlags, mWidgetFlagsMod);

    WidgetList::reverse_iterator anItr = mWidgets.rbegin();
    while (anItr != mWidgets.rend())
    {
        Widget* aWidget = *anItr;

        int aCurFlags = theFlags;
        ModFlags(aCurFlags, aWidget->mWidgetFlagsMod);
        if (belowModal) ModFlags(aCurFlags, mWidgetManager->mBelowModalFlagsMod);

        if (aCurFlags & WIDGETFLAGS_ALLOW_MOUSE)
        {
            if (aWidget->mVisible)
            {
                bool childFound;
                Widget* aCheckWidget = aWidget->GetWidgetAtHelper(x - aWidget->mX, y - aWidget->mY, aCurFlags, &childFound, theWidgetX, theWidgetY);
                if ((aCheckWidget != NULL) || (childFound))
                {
                    *found = true;
                    return aCheckWidget;
                }

                if ((aWidget->mMouseVisible) && (aWidget->GetInsetRect().Contains(x, y)))
                {
                    *found = true;

                    if (aWidget->IsPointVisible(x-aWidget->mX,y-aWidget->mY))
                    {
                        if (theWidgetX)
                            *theWidgetX = x - aWidget->mX;
                        if (theWidgetY)
                            *theWidgetY = y - aWidget->mY;
                        return aWidget;
                    }
                }
            }
        }

        belowModal |= aWidget == mWidgetManager->mBaseModalWidget;

        ++anItr;
    }

    *found = false;
    return NULL;
}

void WidgetContainer::MarkAllDirty()
{
    // TODO: Implement
}

Point WidgetContainer::GetAbsPos()
{
    if (mParent == nullptr)
        return Point(mX,mY);
    else
        return Point(mX,mY) + mParent->GetAbsPos();
}

void WidgetContainer::SysColorChanged() {}

void WidgetContainer::SysColorChangedAll()
{
    // TODO: Implement
}
