#include "Widget.h"

#include "SexyAppBase.h"
#include "WidgetManager.h"

using namespace Sexy;

Widget::Widget()
{

}

Widget::~Widget()
{

}

void Widget::ShowFinger(bool on)
{
    if (mWidgetManager == nullptr)
        return;

    if (on)
        mWidgetManager->mApp->SetCursor(CURSOR_HAND);
    else
        mWidgetManager->mApp->SetCursor(CURSOR_POINTER);
}

bool Widget::WantsFocus()
{
    return mWantsFocus;
}

void Widget::GotFocus()
{
    mHasFocus = true;
}

void Widget::LostFocus()
{
    mHasFocus = false;
}

void Widget::KeyChar(SexyChar theChar)
{
    // TODO: Implement
}

void Widget::KeyDown(KeyCode theKey)
{
    // TODO: Implement
}

void Widget::KeyUp(KeyCode theKey)
{
    // TODO: Implement
}

void Widget::MouseEnter()
{
    // TODO: Implement
}

void Widget::MouseLeave()
{
    // TODO: Implement
}

void Widget::MouseMove(int x, int y)
{
    // TODO: Implement
}

void Widget::MouseDown(int x, int y, int theClickCount)
{
    // TODO: Implement
}

void Widget::MouseDown(int x, int y, int theBtnNum, int theClickCount)
{
    // TODO: Implement
}

void Widget::MouseUp(int x, int y)
{
    // TODO: Implement
}

void Widget::MouseUp(int x, int y, int theClickCount)
{
    // TODO: Implement
}

void Widget::MouseUp(int x, int y, int theBtnNum, int theClickCount)
{
    // TODO: Implement
}

void Widget::MouseDrag(int x, int y)
{
    // TODO: Implement
}

void Widget::MouseWheel(int theDelta)
{
    // TODO: Implement
}

bool Widget::IsPointVisible(int x, int y)
{
    return true;
}

bool Widget::Contains(int theX, int theY)
{
    return ((theX >= mX) && (theX < mX + mWidth) &&
            (theY >= mY) && (theY < mY + mHeight));
}

Rect Widget::GetInsetRect()
{
    return Rect(mX + mMouseInsets.mLeft, mY + mMouseInsets.mTop,
                         mWidth - mMouseInsets.mLeft - mMouseInsets.mRight,
                         mHeight - mMouseInsets.mTop - mMouseInsets.mBottom);
}
