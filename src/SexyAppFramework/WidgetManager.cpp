#include "WidgetManager.h"

using namespace Sexy;

WidgetManager::WidgetManager(SexyAppBase *theApplet)
{
    // TODO: Implement
}

WidgetManager::~WidgetManager()
{
    // TODO: Implement
}

void WidgetManager::Resize(const Rect &theMouseDestRect, const Rect &theMouseSourceRect)
{
    mWidth = theMouseDestRect.mWidth + 2 * theMouseDestRect.mX;
    mHeight = theMouseDestRect.mHeight + 2 * theMouseDestRect.mY;
    mMouseDestRect = theMouseDestRect;
    mMouseSourceRect = theMouseSourceRect;
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

bool WidgetManager::MouseUp(int x, int y, int theClickCount)
{
    // TODO: Implement
    return false;
}

bool WidgetManager::MouseDown(int x, int y, int theClickCount)
{
    // TODO: Implement
    return false;
}

bool WidgetManager::MouseMove(int x, int y)
{
    // TODO: Implement
    return false;
}

bool WidgetManager::MouseDrag(int x, int y)
{
    // TODO: Implement
    return false;
}

bool WidgetManager::MouseExit(int x, int y)
{
    // TODO: Implement
    return false;
}

void WidgetManager::MouseWheel(int theDelta)
{
    // TODO: Implement
}

bool WidgetManager::KeyChar(SexyChar theChar)
{
    // TODO: Implement
    return false;
}

bool WidgetManager::KeyDown(KeyCode key)
{
    // TODO: Implement
    return false;
}

bool WidgetManager::KeyUp(KeyCode key)
{
    // TODO: Implement
    return false;
}

bool WidgetManager::KeyMessageRaw(HWND hwnd, uint message, unsigned long long wparam, long long lparam)
{
    // TODO: Implement
    return false;
}
