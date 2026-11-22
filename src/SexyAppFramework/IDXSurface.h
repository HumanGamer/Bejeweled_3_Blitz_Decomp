#pragma once

#include <ddraw.h>

#include "Common.h"

namespace Sexy
{
    class DDImage;

    class IDXSurface
    {
    public:
        ulong mImageFlags;

    public:
        virtual ~IDXSurface() = default;
        virtual int GetVersion() const = 0;
        virtual HRESULT Lock(void* desc) = 0;
        virtual void Unlock(void* desc) = 0;
        virtual bool GenerateDDSurface(DDImage* image) = 0;
        virtual bool HasSurface() const = 0;
        virtual void* GetSurfacePtr() const = 0;
        virtual void AddRef() = 0;
        virtual void Release() = 0;
        virtual ulong* GetBits(DDImage* image) = 0;
        virtual HDC GetDC() = 0;
        virtual void ReleaseDC(HDC hdc) = 0;
        virtual void SetSurface(void* surface) = 0;
        virtual void GetDimensions(int* width, int* height) = 0;
        virtual HRESULT Blt(LPRECT lpDestRect, void* lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx) = 0;
    };
}
