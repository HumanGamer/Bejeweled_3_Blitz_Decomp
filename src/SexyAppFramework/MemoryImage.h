#pragma once

#include "Common.h"
#include "Image.h"

#define OPTIMIZE_SOFTWARE_DRAWING
#ifdef OPTIMIZE_SOFTWARE_DRAWING
extern bool gOptimizeSoftwareDrawing;
#endif

namespace Sexy
{
    class MemoryImage : public Image
    {
        // TODO: Implement

    public:
        virtual void SetImageMode(bool hasTrans, bool hasAlpha);
    };
}