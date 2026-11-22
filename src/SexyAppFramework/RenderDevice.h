#pragma once

#include "Common.h"
#include "DDInterface.h"

namespace Sexy
{
    class DDInterface;

    class RenderDevice3D
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

        virtual void GetBackBufferDimensions(ulong& width, ulong& height) = 0;

        virtual IUnknown* CreateSurface(int width, int height, bool flush, bool forceARGB8) = 0;
    };
}
