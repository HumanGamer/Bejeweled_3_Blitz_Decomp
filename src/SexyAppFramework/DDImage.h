#pragma once

#include "Common.h"
#include "MemoryImage.h"
#include <ddraw.h>

#include "SharedImage.h"
#include "Rect.h"
#include "Color.h"

namespace Sexy
{
    class IDXSurface;
    class DDInterface;

    class DDImage : public MemoryImage
    {
    public:
        DDInterface* mDDInterface;
        bool mSurfaceSet;
        bool mNoLock;
        bool mWantDDSurface;
        bool mDrawToBits;
        IDXSurface* mSurface;
        int mLockCount;
        DDSURFACEDESC mLockedSurfaceDesc;
    public:

        // TODO: Implement

        void CommitBits() override;

        virtual void NormalFillRect(const Rect& theRect, const Color& theColor);

    public:
        DDImage();
        DDImage(DDInterface* ddInterface);
        virtual ~DDImage();

        virtual void SetSurface(LPDIRECTDRAWSURFACE theSurface);

        // TODO: Implement
    };
}
