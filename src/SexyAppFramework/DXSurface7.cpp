#include "DXSurface7.h"

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
    // TODO: Implement
    return false;
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
