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
