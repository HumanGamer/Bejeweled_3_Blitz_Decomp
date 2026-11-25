#include "DXSurface9.h"

#include "AutoCrit.h"
#include "DDInterface.h"
#include "DDImage.h"

using namespace Sexy;

DXSurface9::DXSurface9(DDInterface* ddInterface)
{
    // TODO: Implement
}

DXSurface9::~DXSurface9()
{
    // TODO: Implement
}

int DXSurface9::GetVersion() const
{
    return 9;
}

HRESULT DXSurface9::Lock(void* desc)
{
    // TODO: Implement
    return 0;
}

void DXSurface9::Unlock(void* desc)
{
    // TODO: Implement
}

bool DXSurface9::GenerateDDSurface(DDImage* image)
{
    // TODO: Implement
    return false;
}

bool DXSurface9::HasSurface() const
{
    // TODO: Implement
    return false;
}

void* DXSurface9::GetSurfacePtr() const
{
    // TODO: Implement
    return nullptr;
}

void DXSurface9::AddRef()
{
    // TODO: Implement
}

void DXSurface9::Release()
{
    // TODO: Implement
}

ulong* DXSurface9::GetBits(DDImage* image)
{
    // TODO: Implement
    return nullptr;
}

HDC DXSurface9::GetDC()
{
    // TODO: Implement
    return nullptr;
}

void DXSurface9::ReleaseDC(HDC hdc)
{
    // TODO: Implement
}

void DXSurface9::SetSurface(void* surface)
{
    // TODO: Implement
}

void DXSurface9::GetDimensions(int* width, int* height)
{
    // TODO: Implement
    *width = 0;
    *height = 0;
}

HRESULT DXSurface9::Blt(LPRECT lpDestRect, void* lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
    // TODO: Implement
    return 0;
}

IDXSurface* CreateDXSurface9(DDInterface* ddInterface)
{
    return new DXSurface9(ddInterface);
}
