#include "DXSurface7.h"

#include "AutoCrit.h"
#include "DDInterface.h"
#include "DDImage.h"

using namespace Sexy;

DXSurface7::DXSurface7(DDInterface* ddInterface)
{
    mDDInterface = ddInterface;
    mSurface = nullptr;
}

DXSurface7::~DXSurface7()
{
    if (mSurface != nullptr)
        mSurface->Release();
}

int DXSurface7::GetVersion() const
{
    return 7;
}

HRESULT DXSurface7::Lock(void* desc)
{
    if (mSurface != nullptr)
        return mSurface->Lock(nullptr, (LPDDSURFACEDESC)desc, DDLOCK_WAIT, nullptr);

    return -1;
}

void DXSurface7::Unlock(void* desc)
{
    if (mSurface != nullptr)
        mSurface->Unlock(desc);
}

bool DXSurface7::GenerateDDSurface(DDImage* image)
{
    if (mSurface != nullptr)
        return false;

    image->CommitBits();

    if (image->mHasAlpha)
        return false;

    image->mWantDDSurface = true;

    if (image->mColorTable != nullptr || image->mNativeAlphaData != nullptr)
        image->GetBits();

    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = image->mWidth;
    ddsd.dwHeight = image->mHeight;

    AutoCrit aCrit(mDDInterface->mCritSect);

    HRESULT aResult = mDDInterface->CreateSurface(&ddsd, &mSurface, nullptr);
    if (aResult != DD_OK)
        return false;

    DDSURFACEDESC lockedSurfaceDesc;
    ZeroMemory(&lockedSurfaceDesc, sizeof(lockedSurfaceDesc));
    lockedSurfaceDesc.dwSize = sizeof(lockedSurfaceDesc);
    if (mSurface->Lock(nullptr, &lockedSurfaceDesc, DDLOCK_WAIT, nullptr) < DD_OK)
    {
        mSurface->Release();
        mSurface = nullptr;
        return false;
    }

    const int rRightShift = 16 + (8 - mDDInterface->mRedBits);
    const int gRightShift = 8 + (8 - mDDInterface->mGreenBits);
    const int bRightShift = 0 + (8 - mDDInterface->mBlueBits);

    const int rLeftShift = mDDInterface->mRedShift;
    const int gLeftShift = mDDInterface->mGreenShift;
    const int bLeftShift = mDDInterface->mBlueShift;

    const int rMask = lockedSurfaceDesc.ddpfPixelFormat.dwRBitMask;
    const int gMask = lockedSurfaceDesc.ddpfPixelFormat.dwGBitMask;
    const int bMask = lockedSurfaceDesc.ddpfPixelFormat.dwBBitMask;

	int aNumBits = lockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;

    if (aNumBits == 16)
    {
		ushort* mSurfaceBits = (ushort*) lockedSurfaceDesc.lpSurface;

        if (mSurfaceBits != nullptr)
        {
            int i;
            bool firstTrans = true;

            ushort* a16Bits = nullptr;

            if (image->mBits != nullptr)
            {
                a16Bits = new ushort[image->mWidth * image->mHeight];
                ulong* aSrcPtr = image->mBits;
                ushort* a16SrcPtr = a16Bits;

                for (i = 0; i < image->mWidth * image->mHeight; i++)
                {
                    ulong val = *(aSrcPtr++);

                    *a16SrcPtr =	(((val>>rRightShift)<<rLeftShift)&rMask) |
                                    (((val>>gRightShift)<<gLeftShift)&gMask) |
                                    (((val>>bRightShift)<<bLeftShift)&bMask);

                    int anAlpha = val >> 24;
                    if (firstTrans && anAlpha < 255)
                        firstTrans = false;

                    ++a16SrcPtr;
                }

                if (a16Bits != nullptr)
                {
                    ushort* aDestPtr = mSurfaceBits;
                    ushort* a16SrcPtr = a16Bits;
                    for (int aRow = 0; aRow < image->mHeight; aRow++)
                    {
                        for (int aCol = 0; aCol < image->mWidth; aCol++)
                        {
                            *(aDestPtr++) = *a16SrcPtr;
                            ++a16SrcPtr;
                        }

                        aDestPtr += lockedSurfaceDesc.lPitch/2 - image->mWidth;
                    }
                }

                delete a16Bits;
            }
        }
    }
    else if (aNumBits == 32 && lockedSurfaceDesc.lpSurface != nullptr && image->mBits != nullptr)
    {
		ulong* mSurfaceBits = (ulong*) lockedSurfaceDesc.lpSurface;

        ulong* aDestPtr = mSurfaceBits;
        ulong* aSrcPtr = image->mBits;
        for (int aRow = 0; aRow < image->mHeight; aRow++)
        {
            for (int aCol = 0; aCol < image->mWidth; aCol++)
            {
                ulong val = *(aSrcPtr++);

                *(aDestPtr++) =
                        (((val>>rRightShift)<<rLeftShift)&rMask) |
                        (((val>>gRightShift)<<gLeftShift)&gMask) |
                        (((val>>bRightShift)<<bLeftShift)&bMask);
            }

            aDestPtr += lockedSurfaceDesc.lPitch/4 - image->mWidth;
        }
    }

    mSurface->Unlock(nullptr);

    return true;
}

bool DXSurface7::HasSurface() const
{
    return mSurface != nullptr;
}

void* DXSurface7::GetSurfacePtr() const
{
    return mSurface;
}

void DXSurface7::AddRef()
{
    if (mSurface != nullptr)
        mSurface->AddRef();
}

void DXSurface7::Release()
{
    if (mSurface != nullptr)
    {
        mSurface->Release();
        mSurface = nullptr;
    }
}

ulong* DXSurface7::GetBits(DDImage* image)
{
    // TODO: Implement
    return nullptr;
}

HDC DXSurface7::GetDC()
{
    if (mSurface == nullptr)
        return nullptr;

    HDC dc;
    HRESULT hr = mSurface->GetDC(&dc);

    if (FAILED(hr))
        return nullptr;

    return dc;
}

void DXSurface7::ReleaseDC(HDC hdc)
{
    if (mSurface != nullptr)
        mSurface->ReleaseDC(hdc);
}

void DXSurface7::SetSurface(void* surface)
{
    if (mSurface != surface)
        mSurface = (LPDIRECTDRAWSURFACE)surface;
}

void DXSurface7::GetDimensions(int* width, int* height)
{
    *width = 0;
    *height = 0;

    if (mSurface == nullptr)
        return;

    DDSURFACEDESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
    if (SUCCEEDED(mSurface->GetSurfaceDesc(&ddsd)))
    {
        *width = ddsd.dwWidth;
        *height = ddsd.dwHeight;
    }
}

HRESULT DXSurface7::Blt(LPRECT lpDestRect, void* lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
    // TODO: in the original binary it does lpDDSrcSurface[3], this needs to be verified.
    if (mSurface != nullptr)
        mSurface->Blt(lpDestRect, (LPDIRECTDRAWSURFACE)lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);

    return -1;
}
