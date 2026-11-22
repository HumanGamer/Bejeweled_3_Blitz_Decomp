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

        static std::string sErrorString;
    };
}
