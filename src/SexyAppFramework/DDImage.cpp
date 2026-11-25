#include "DDImage.h"

#include "Debug.h"
#include "DXSurface7.h"
#include "DXSurface8.h"
#include "DXSurface9.h"
#include "IDXSurface.h"
#include "SexyAppBase.h"

using namespace Sexy;

DDImage::DDImage()
{
    mDDInterface = gSexyAppBase->mDDInterface;

    Init();
}

DDImage::DDImage(DDInterface *ddInterface)
{
    mDDInterface = ddInterface;

    Init();
}

DDImage::~DDImage()
{
    if (mSurface != nullptr)
        mSurface->Release();

    mDDInterface->RemoveDDImage(this);

	DBG_ASSERTE(mLockCount == 0);
}

void DDImage::Init()
{
    mSurface = nullptr;
    mDDInterface->AddDDImage(this);

    mNoLock = false;
    mWantDDSurface = false;
    mDrawToBits = false;
    mSurfaceSet = false;

    mLockCount = 0;
}

void DDImage::SetSurface(LPDIRECTDRAWSURFACE theSurface)
{
    mSurfaceSet = true;

    if (mSurface != nullptr)
    {
        int version;
        if (mDDInterface->mIsD3D8Or9)
        {
            if (mDDInterface->mIsD3D9)
                version = 9;
            else
                version = 8;
        }
        else
        {
            version = 7;
        }

        if (mSurface->GetVersion() != version)
        {
            // TODO: This might actually just be mSurface->Release(); without the check and setting to nullptr
            if (mSurface != nullptr)
                mSurface->Release();
            mSurface = nullptr;
        }
    }

    if (theSurface != nullptr)
    {
        if (mSurface == nullptr)
        {
            if (mDDInterface->mIsD3D9)
                mSurface = CreateDXSurface9(mDDInterface);
            else if (mDDInterface->mIsD3D8)
                mSurface = CreateDXSurface8(mDDInterface);
            else
                mSurface = CreateDXSurface7(mDDInterface);
        }

        mSurface->SetSurface(theSurface);
        mSurface->GetDimensions(&mWidth, &mHeight);
        mNoLock = false;
    }
    else
    {
        // TODO: This might actually just be mSurface->Release(); without the check and setting to nullptr
        if (mSurface != nullptr)
            mSurface->Release();
        mSurface = nullptr;

        mHeight = 0;
        mWidth = 0;
        mNoLock = true;
    }
}

void DDImage::CommitBits()
{
    if (mSurface == nullptr)
    {
        MemoryImage::CommitBits();
        return;
    }
}

ulong *DDImage::GetBits()
{
    if (mBits == nullptr)
    {
        if (mSurface == nullptr)
            return MemoryImage::GetBits();

        if (mNoLock)
            return nullptr;

        mBits = mSurface->GetBits(this);
    }

    return mBits;
}

void DDImage::NormalFillRect(const Rect &theRect, const Color &theColor)
{
    // TODO: Implement
}
