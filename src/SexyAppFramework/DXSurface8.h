#pragma once

#include "Common.h"
#include "IDXSurface.h"

struct IDirect3DResource8;
struct IDirect3DSurface8;

namespace Sexy
{
    class D3D8Interface;
    class D3D9Interface;
    class DDInterface;

    class DXSurface8 : public IDXSurface
    {
    public:
        DDInterface* mDDInterface;
        D3D8Interface* mD3DInterface;
        IDirect3DResource8* mSurface;
        IDirect3DSurface8* mTextureSurface;
        DWORD mSurfaceFormat;
        int mRedBits;
        int mRedShift;
        int mRedMask;
        int mGreenBits;
        int mGreenShift;
        int mGreenMask;
        int mBlueBits;
        int mBlueShift;
        int mBlueMask;
        bool mIsTexture;

    public:
        DXSurface8(DDInterface* ddInterface);
        ~DXSurface8() override;

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

Sexy::IDXSurface* CreateDXSurface8(Sexy::DDInterface* ddInterface);
