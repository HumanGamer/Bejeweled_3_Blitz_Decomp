#pragma once

#include "Common.h"
#include "MemoryImage.h"
#include <ddraw.h>

#include "SharedImage.h"
#include "Rect.h"
#include "Color.h"

namespace Sexy
{
    class DDInterface;

    class DDImage : public MemoryImage
    {
    public:
        // TODO: Implement

        virtual void NormalFillRect(const Rect& theRect, const Color& theColor);

    public:
        DDImage();
        DDImage(DDInterface* ddInterface);
        virtual ~DDImage();

        virtual void SetSurface(LPDIRECTDRAWSURFACE theSurface);

        // TODO: Implement
    };
}
