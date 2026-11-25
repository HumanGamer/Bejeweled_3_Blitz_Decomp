#pragma once

#include "Common.h"
#include "IDXSurface.h"

namespace Sexy
{
    class DDInterface;

    class DXSurface7 : public IDXSurface
    {
    public:
        DDInterface* mDDInterface;
        LPDIRECTDRAWSURFACE mSurface;

    public:
        DXSurface7(DDInterface* ddInterface);
        ~DXSurface7() override;

        int GetVersion() const override;
        HRESULT Lock(void* desc) override;
        void Unlock(void* desc) override;
        bool GenerateDDSurface(DDImage* image) override;
        bool HasSurface() const override;
        void* GetSurfacePtr() const override;
        void AddRef() override;
        void Release() override;
        ulong* GetBits(DDImage* image) override;
        HDC GetDC() override;
        void ReleaseDC(HDC hdc) override;
        void SetSurface(void* surface) override;
        void GetDimensions(int* width, int* height) override;
        HRESULT Blt(LPRECT lpDestRect, void* lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx) override;
    };

}

Sexy::DXSurface7* CreateDXSurface7(Sexy::DDInterface* ddInterface);
