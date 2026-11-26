#pragma once

#include "Common.h"
#include "D3DInterface.h"

namespace Sexy
{
    class D3D9Interface : public D3DInterface
    {
    public:
        D3D9Interface();
        ~D3D9Interface() override;

        ECapsFlags GetCapsFlags() override;

        void Cleanup() override;
        bool Flush(ulong flags) override;
        bool Present(const RECT* src, const RECT* dst) override;

        bool InitFromDDInterface(DDInterface *ddInterface, DDInterface::EResult *result, bool testOnly, bool *is3DRecommended) override;

        std::string GetInfoString(EInfoString infoType) override;
        void GetBackBufferDimensions(ulong &width, ulong &height) override;


        IUnknown *CreateSurface(int width, int height, bool flush, bool forceARGB8) override;

        bool CanBltSurface(bool unknown) override;
        void BltSurface(IUnknown *surface, const Rect& src, const Rect& dst) override;

        void ClearColorBuffer(const Color &color) override;
    };

    D3D9Interface* CreateD3D9Interface();
}
