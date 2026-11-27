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
    protected:
        void DeleteAllNonSurfaceData();
    public:
        DDInterface* mDDInterface;
        bool mSurfaceSet;
        bool mNoLock;
        bool mWantDDSurface;
        bool mDrawToBits;
        IDXSurface* mSurface;
        int mLockCount;
        DDSURFACEDESC mLockedSurfaceDesc;

    private:
        void Init();

    public:

        // TODO: Implement

	    IDXSurface* GetSurface();
	    void BitsChanged() override;
        void CommitBits() override;

        ulong *GetBits() override;

        void FillRect(const Rect &theRect, const Color &theColor, int theDrawMode) override;

        virtual void NormalFillRect(const Rect& theRect, const Color& theColor);
	    virtual void AdditiveFillRect(const Rect& theRect, const Color& theColor);

    public:
        DDImage();
        DDImage(DDInterface* ddInterface);
        virtual ~DDImage();

        virtual bool LockSurface();
        virtual bool UnlockSurface();

        virtual void SetSurface(LPDIRECTDRAWSURFACE theSurface);

        // TODO: Implement
    };
}
