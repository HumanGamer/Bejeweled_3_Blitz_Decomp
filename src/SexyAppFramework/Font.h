#pragma once

#include "Common.h"

namespace Sexy
{
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

        // TODO: Implement
    };
}