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

        HRenderContext CreateContext(Image *theImage, const HRenderContext &theContext) override;
        void DeleteContext(const HRenderContext &context) override;
        void SetCurrentContext(const HRenderContext &context) override;

        void FillRect(const Rect &theRect, const Color &theColor, int theDrawMode) override;

        void Cleanup() override;

        bool SceneBegun() override;

        void RemoveImageRenderData(MemoryImage *theImage) override;

        IUnknown *GetBackBufferSurface() override;

        bool SetRenderTargetSurface(IUnknown *surface) override;

        bool RecoverImageBitsFromRenderData(MemoryImage *theImage) override;

        static std::string sErrorString;
    };
}
