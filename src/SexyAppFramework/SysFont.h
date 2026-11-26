#pragma once

#include "Common.h"
#include "Font.h"

namespace Sexy
{
    class SexyAppBase;

    class SysFont : public Font
    {
    public:

        // TODO: Implement

    public:
        SysFont(const std::string& theFace, int thePointSize, bool bold = false, bool italics = false, bool underline = false);
        SysFont(SexyAppBase* theApp, const std::string& theFace, int thePointSize, int theScript = ANSI_CHARSET, bool bold = false, bool italics = false, bool underline = false);
        SysFont(const SysFont& theSysFont);

        ~SysFont() override;

        // TODO: Implement

        int StringWidth(const SexyString& theString) override;

        // TODO: Implement
    };
}