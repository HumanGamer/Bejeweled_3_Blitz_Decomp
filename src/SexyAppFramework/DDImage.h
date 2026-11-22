#pragma once

#include "Common.h"
#include "MemoryImage.h"
#include <ddraw.h>

#include "SharedImage.h"

namespace Sexy
{
    class DDInterface;

    class DDImage : public MemoryImage
    {
    public:
        // TODO: Implement

    public:
        DDImage();
        DDImage(DDInterface* ddInterface);
        virtual ~DDImage();

        virtual void SetSurface(LPDIRECTDRAWSURFACE theSurface);

        // TODO: Implement
    };
}
