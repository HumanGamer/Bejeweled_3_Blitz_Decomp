#include "DXSurface8.h"

#include "AutoCrit.h"
#include "DDInterface.h"
#include "DDImage.h"

using namespace Sexy;

DXSurface8::DXSurface8(DDInterface* ddInterface)
{
    // TODO: Implement
}

DXSurface8::~DXSurface8()
{
    // TODO: Implement
}

int DXSurface8::GetVersion() const
{
    return 8;
}

HRESULT DXSurface8::Lock(void* desc)
{
    // TODO: Implement
    return 0;
}

void DXSurface8::Unlock(void* desc)
{
    // TODO: Implement
}

bool DXSurface8::GenerateDDSurface(DDImage* image)
{
    // TODO: Implement
    return false;
}

bool DXSurface8::HasSurface() const
{
    // TODO: Implement
    return false;
}

void* DXSurface8::GetSurfacePtr() const
{
    // TODO: Implement
    return nullptr;
}

void DXSurface8::AddRef()
{
    // TODO: Implement
}

void DXSurface8::Release()
{
    // TODO: Implement
}

ulong* DXSurface8::GetBits(DDImage* image)
{
    // TODO: Implement
    return nullptr;
}

HDC DXSurface8::GetDC()
{
    // TODO: Implement
    return nullptr;
}

void DXSurface8::ReleaseDC(HDC hdc)
{
    // TODO: Implement
}

void DXSurface8::SetSurface(void* surface)
{
    // TODO: Implement
}

void DXSurface8::GetDimensions(int* width, int* height)
{
    // TODO: Implement
    *width = 0;
    *height = 0;
}

HRESULT DXSurface8::Blt(LPRECT lpDestRect, void* lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
    // TODO: Implement
    return 0;
}

IDXSurface* CreateDXSurface8(DDInterface* ddInterface)
{
    return new DXSurface8(ddInterface);
}
