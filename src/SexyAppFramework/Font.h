#pragma once

#include "Color.h"
#include "Common.h"
#include "Rect.h"

namespace Sexy
{
    class Graphics;

    class Font
    {
    public:
        // TODO: Implement

    public:
        Font();
        Font(const Font& theFont);
        virtual ~Font();

        // TODO: Implement

        virtual int StringWidth(const SexyString& theString);

        virtual void DrawString(Graphics* g, int theX, int theY, const SexyString& theString, const Color& theColor, const Rect& theClipRect);

        // TODO: Implement
    };
}
