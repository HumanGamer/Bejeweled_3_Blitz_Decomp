#pragma once

#include "Common.h"
#include "RenderDevice.h"

namespace Sexy
{
    class D3DInterface : public RenderDevice3D
    {
    public:
        D3DInterface();
        virtual ~D3DInterface();

        void Cleanup() override;

        bool SceneBegun() override;

        IUnknown *GetBackBufferSurface() override;

        bool SetRenderTargetSurface(IUnknown *surface) override;

        static std::string sErrorString;
    };
}
