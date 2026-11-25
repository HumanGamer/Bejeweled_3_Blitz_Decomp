#include "WidgetManager.h"

using namespace Sexy;

WidgetManager::WidgetManager(SexyAppBase *theApp)
{
    mApp = theApp;

    mMinDeferredOverlayPriority = 0x7FFFFFFF;
    mWidgetManager = this;
    mMouseIn = false;
    mDefaultTab = nullptr;
    mImage = nullptr;
    mLastHadTransients = false;
    mPopupCommandWidget = nullptr;
    mFocusWidget = nullptr;
    mLastDownWidget = nullptr;
    mOverWidget = nullptr;
    mBaseModalWidget = nullptr;
    mDefaultBelowModalFlagsMod.mRemoveFlags = WIDGETFLAGS_ALLOW_MOUSE | WIDGETFLAGS_ALLOW_FOCUS;
    mWidth = 0;
    mHeight = 0;
    mHasFocus = true;
    mUpdateCnt = 0;
    mLastDownButtonId = 0;
    mDownButtons = 0;
    mActualDownButtons = 0;
    mWidgetFlags = WIDGETFLAGS_UPDATE | WIDGETFLAGS_DRAW | WIDGETFLAGS_CLIP |
        WIDGETFLAGS_ALLOW_MOUSE | WIDGETFLAGS_ALLOW_FOCUS;

    for (int i = 0; i < 0xFF; i++)
        mKeyDown[i] = false;
}

WidgetManager::~WidgetManager()
{
	FreeResources();
}

void WidgetManager::FreeResources()
{
    // Empty...
}

void WidgetManager::Resize(const Rect &theMouseDestRect, const Rect &theMouseSourceRect)
{
    mWidth = theMouseDestRect.mWidth + 2 * theMouseDestRect.mX;
    mHeight = theMouseDestRect.mHeight + 2 * theMouseDestRect.mY;
    mMouseDestRect = theMouseDestRect;
    mMouseSourceRect = theMouseSourceRect;
}

int WidgetManager::GetWidgetFlags()
{
    return mHasFocus ? mWidgetFlags : GetModFlags(mWidgetFlags, mLostFocusFlagsMod);
}

Widget* WidgetManager::GetAnyWidgetAt(int x, int y, int* theWidgetX, int* theWidgetY)
{
    bool found;
    return GetWidgetAtHelper(x, y, GetWidgetFlags(), &found, theWidgetX, theWidgetY);
}

Widget *WidgetManager::GetWidgetAt(int x, int y, int *theWidgetX, int *theWidgetY)
{
    Widget* aWidget = GetAnyWidgetAt(x, y, theWidgetX, theWidgetY);
    if ((aWidget != nullptr) && (aWidget->mDisabled))
        aWidget = nullptr;
    return aWidget;
}

void WidgetManager::SetFocus(Widget *aWidget)
{
    if (aWidget==mFocusWidget)
        return;

    if (mFocusWidget != nullptr)
        mFocusWidget->LostFocus();

    if ((aWidget != nullptr) && (aWidget->mWidgetManager == this))
    {
        mFocusWidget = aWidget;

        if ((mHasFocus) && (mFocusWidget != nullptr))
            mFocusWidget->GotFocus();
    }
    else
        mFocusWidget = nullptr;
}

void WidgetManager::DoMouseUps()
{
    // TODO: Implement
}

bool WidgetManager::UpdateFrame()
{
    // TODO: Implement
    return false;
}

bool WidgetManager::UpdateFrameF(float theFrac)
{
    // TODO: Implement
    return false;
}

void WidgetManager::SetPopupCommandWidget(Widget* theList)
{
    mPopupCommandWidget = theList;
    AddWidget(mPopupCommandWidget);
}

void WidgetManager::RemovePopupCommandWidget()
{
    if (mPopupCommandWidget != NULL)
    {
        Widget *aWidget = mPopupCommandWidget;
        mPopupCommandWidget = NULL;
        RemoveWidget(aWidget);
    }
}

void WidgetManager::MousePosition(int x, int y)
{
    int aLastMouseX = mLastMouseX;
    int aLastMouseY = mLastMouseY;

    mLastMouseX = x;
    mLastMouseY = y;

    int aWidgetX;
    int aWidgetY;
    Widget* aWidget = GetWidgetAt(x, y, &aWidgetX, &aWidgetY);

    if (aWidget != mOverWidget)
    {
        Widget* aLastOverWidget = mOverWidget;
        mOverWidget = nullptr;

        if (aLastOverWidget != nullptr)
            MouseLeave(aLastOverWidget);

        mOverWidget = aWidget;
        if (aWidget != nullptr)
        {
            MouseEnter(aWidget);
            aWidget->MouseMove(aWidgetX, aWidgetY);
        }
    }
    else if ((aLastMouseX != x) || (aLastMouseY != y))
    {
        if (aWidget != nullptr)
            aWidget->MouseMove(aWidgetX, aWidgetY);
    }
}

void WidgetManager::RehupMouse()
{
    if (mLastDownWidget != nullptr)
    {
        if (mOverWidget != nullptr)
        {
            Widget* aWidgetOver = GetWidgetAt(mLastMouseX, mLastMouseY, nullptr, nullptr);

            if (aWidgetOver != mLastDownWidget)
            {
                Widget* anOverWidget = mOverWidget;
                mOverWidget = nullptr;
                MouseLeave(anOverWidget);
            }
        }
    }
    else if (mMouseIn)
        MousePosition(mLastMouseX, mLastMouseY);
}

void WidgetManager::RemapMouse(int &theX, int &theY)
{
    int width = mMouseSourceRect.mWidth;
    if (width != 0)
    {
        int height = mMouseSourceRect.mHeight;
        if (height != 0)
        {
            theX = mMouseDestRect.mX + mMouseDestRect.mWidth * (theX - mMouseSourceRect.mX) / width;
            theY = mMouseDestRect.mY + mMouseDestRect.mHeight * (theY - mMouseSourceRect.mY) / height;
        }
    }
}

void WidgetManager::MouseEnter(Widget* theWidget)
{
    theWidget->mIsOver = true;

    theWidget->MouseEnter();
    if (theWidget->mDoFinger)
        theWidget->ShowFinger(true);
}

void WidgetManager::MouseLeave(Widget* theWidget)
{
    theWidget->mIsOver = false;

    theWidget->MouseLeave();
    if (theWidget->mDoFinger)
        theWidget->ShowFinger(false);
}

bool WidgetManager::MouseUp(int x, int y, int theClickCount)
{
    mLastInputUpdateCnt = mUpdateCnt;

    int aMask;

    if (theClickCount < 0)
        aMask = 0x02;
    else if (theClickCount == 3)
        aMask = 0x04;
    else
        aMask = 0x01;

    // Make sure that we thought this button was down anyway - possibly not, if we
    //  disabled the widget already or something
    mActualDownButtons &= ~aMask;
    if ((mLastDownWidget != nullptr) && ((mDownButtons & aMask) != 0))
    {
        Widget* aLastDownWidget = mLastDownWidget;

        mDownButtons &= ~aMask;
        if (mDownButtons == 0)
            mLastDownWidget = nullptr;

        aLastDownWidget->mIsDown = false;
        aLastDownWidget->MouseUp(x - aLastDownWidget->mX, y - aLastDownWidget->mY, theClickCount);
    }
    else
        mDownButtons &= ~aMask;

    MousePosition(x, y);

    return true;
}

bool WidgetManager::MouseDown(int x, int y, int theClickCount)
{
    mLastInputUpdateCnt = mUpdateCnt;

    if (theClickCount < 0)
        mActualDownButtons |= 0x02;
    else if (theClickCount == 3)
        mActualDownButtons |= 0x04;
    else
        mActualDownButtons |= 0x01;

    MousePosition(x, y);

    if ((mPopupCommandWidget != nullptr) && (!mPopupCommandWidget->Contains(x, y)))
        RemovePopupCommandWidget();

    int aWidgetX;
    int aWidgetY;
    Widget* aWidget = GetWidgetAt(x, y, &aWidgetX, &aWidgetY);

    // Begin mouse down options
    /*
        // Option 1
        //This code sets a new widget as the mouse drag focus widget and lets the old
        //mousedownwidget think the buttons popped up.
        if ((mLastDownWidget != NULL) && (mLastDownWidget != aWidget))
        {
            DoMouseUps(mLastDownWidget, mDownButtons);
            mDownButtons = 0;
        }
    */
    // Option 2
    // This code passes all button downs to the mLastDownWidget
    if (mLastDownWidget != nullptr)
        aWidget = mLastDownWidget;

    // End mouse down options

    if (theClickCount < 0)
    {
        mLastDownButtonId = -1;
        mDownButtons |= 0x02;
    }
    else if (theClickCount == 3)
    {
        mLastDownButtonId = 2;
        mDownButtons |= 0x04;
    }
    else
    {
        mLastDownButtonId = 1;
        mDownButtons |= 0x01;
    }

    mLastDownWidget = aWidget;
    if (aWidget != nullptr)
    {
        if (aWidget->WantsFocus())
            SetFocus(aWidget);

        aWidget->mIsDown = true;
        aWidget->MouseDown(aWidgetX, aWidgetY, theClickCount);
    }

    return true;
}

bool WidgetManager::MouseMove(int x, int y)
{
    mLastInputUpdateCnt = mUpdateCnt;

    if (mDownButtons)
        return MouseDrag(x,y);

    mMouseIn = true;
    MousePosition(x, y);

    return true;
}

bool WidgetManager::MouseDrag(int x, int y)
{
    mLastInputUpdateCnt = mUpdateCnt;

    mMouseIn = true;
    mLastMouseX = x;
    mLastMouseY = y;

    if ((mOverWidget != nullptr) && (mOverWidget != mLastDownWidget))
    {
        Widget* anOverWidget = mOverWidget;
        mOverWidget = nullptr;
        MouseLeave(anOverWidget);
    }

    if (mLastDownWidget != nullptr)
    {
        Point anAbsPos = mLastDownWidget->GetAbsPos();

        int aWidgetX = x - anAbsPos.mX;
        int aWidgetY = y - anAbsPos.mY;
        mLastDownWidget->MouseDrag(aWidgetX, aWidgetY);

        Widget* aWidgetOver = GetWidgetAt(x, y, nullptr, nullptr);

        if ((aWidgetOver == mLastDownWidget) && (aWidgetOver != nullptr))
        {
            if (mOverWidget == nullptr)
            {
                mOverWidget = mLastDownWidget;
                MouseEnter(mOverWidget);
            }
        }
        else
        {
            if (mOverWidget != nullptr)
            {
                Widget* anOverWidget = mOverWidget;
                mOverWidget = nullptr;
                MouseLeave(anOverWidget);
            }
        }
    }

    return true;
}

bool WidgetManager::MouseExit(int x, int y)
{
    mLastInputUpdateCnt = mUpdateCnt;

    mMouseIn = false;

    if (mOverWidget != nullptr)
    {
        MouseLeave(mOverWidget);
        mOverWidget = nullptr;
    }

    return true;
}

void WidgetManager::MouseWheel(int theDelta)
{
    mLastInputUpdateCnt = mUpdateCnt;

    if (mFocusWidget != nullptr)
        mFocusWidget->MouseWheel(theDelta);
}

bool WidgetManager::KeyChar(SexyChar theChar)
{
    mLastInputUpdateCnt = mUpdateCnt;

    if (theChar == KEYCODE_TAB)
    {
        if (mKeyDown[KEYCODE_CONTROL])
        {
            if (mDefaultTab != nullptr)
                mDefaultTab->KeyChar(theChar);

            return true;
        }
    }

    if (mFocusWidget != nullptr)
        mFocusWidget->KeyChar(theChar);

    return true;
}

bool WidgetManager::KeyDown(KeyCode key)
{
    mLastInputUpdateCnt = mUpdateCnt;

    if ((key >= 0) && (key < 0xFF))
        mKeyDown[key] = true;

    if (mFocusWidget != nullptr)
        mFocusWidget->KeyDown(key);

    return true;
}

bool WidgetManager::KeyUp(KeyCode key)
{
    mLastInputUpdateCnt = mUpdateCnt;

    if ((key >= 0) && (key < 0xFF))
        mKeyDown[key] = false;

    if ((key == KEYCODE_TAB) && (mKeyDown[KEYCODE_CONTROL]))
        return true;

    if (mFocusWidget != nullptr)
        mFocusWidget->KeyUp(key);

    return true;
}

bool WidgetManager::KeyMessageRaw(HWND hwnd, uint message, unsigned long long wparam, long long lparam)
{
    // TODO: Implement
    return false;
}
