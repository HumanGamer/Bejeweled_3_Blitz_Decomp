#pragma once

#include "Common.h"

namespace Sexy
{
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
    };
}