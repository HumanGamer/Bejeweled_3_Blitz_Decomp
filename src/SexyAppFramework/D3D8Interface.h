#pragma once

#include "Common.h"
#include "D3DInterface.h"

namespace Sexy
{
    class D3D8Interface : public D3DInterface
    {
    public:
        D3D8Interface();
        ~D3D8Interface() override;

        ECapsFlags GetCapsFlags() override;

        bool InitFromDDInterface(DDInterface *ddInterface, DDInterface::EResult *result, bool testOnly, bool *is3DRecommended) override;

        void GetBackBufferDimensions(ulong &width, ulong &height) override;

        IUnknown *CreateSurface(int width, int height, bool flush, bool forceARGB8) override;
    };

    D3D8Interface* CreateD3D8Interface();
}
