#pragma once

#include "Color.h"
#include "Common.h"
#include "DDInterface.h"

namespace Sexy
{
    class DDInterface;

    class RenderDevice
    {
    public:
        struct Span
        {
            int mY;
            int mX;
            int mWidth;
        };

        // TODO: Implement
    };

    class RenderDevice3D : public RenderDevice
    {
    public:
        enum ECapsFlags
        {
            CAPF_PixelShaders = 0x2,
            CAPF_VertexShaders = 0x4,
            CAPF_ImageRenderTargets = 0x8,
            CAPF_AutoWindowedVSync = 0x10,
            CAPF_CubeMaps = 0x20,
            CAPF_VolumeMaps = 0x40,
        };

        virtual ECapsFlags GetCapsFlags() = 0;

        virtual bool InitFromDDInterface(DDInterface* ddInterface, DDInterface::EResult* result, bool testOnly, bool* is3DRecommended) = 0;
        virtual void Cleanup() = 0;
        virtual bool Flush(ulong flags) = 0;
        virtual bool Present(const RECT* src, const RECT* dst) = 0;

        virtual void GetBackBufferDimensions(ulong& width, ulong& height) = 0;
        virtual bool SceneBegun() = 0;

        virtual IUnknown* GetBackBufferSurface() = 0;
        virtual IUnknown* CreateSurface(int width, int height, bool flush, bool forceARGB8) = 0;
        virtual bool CanBltSurface(bool unknown) = 0; // TODO: Figure out argument name
        virtual void BltSurface(IUnknown* surface, const Rect& src, const Rect& dst) = 0;
        virtual bool SetRenderTargetSurface(IUnknown* surface) = 0;

        virtual void ClearColorBuffer(const Color& color) = 0;
    };
}
