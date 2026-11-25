#include "DDImage.h"

using namespace Sexy;

DDImage::DDImage()
{
    // TODO: Implement
}

DDImage::DDImage(DDInterface *ddInterface)
{
    // TODO: Implement
}

DDImage::~DDImage()
{
    // TODO: Implement
}

void DDImage::SetSurface(LPDIRECTDRAWSURFACE theSurface)
{
    // TODO: Implement
}

void DDImage::CommitBits()
{
    if (mSurface == nullptr)
    {
        MemoryImage::CommitBits();
        return;
    }
}

void DDImage::NormalFillRect(const Rect &theRect, const Color &theColor)
{
    // TODO: Implement
}
