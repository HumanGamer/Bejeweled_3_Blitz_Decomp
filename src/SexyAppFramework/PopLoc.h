#pragma once

#include "Common.h"

namespace Sexy
{
    typedef std::map<int, std::wstring> IntWStringMap;
    class PopLoc
    {
    public:
        IntWStringMap mIdStrings;
        WStringWStringMap mNameStrings; // unused?

    public:
        PopLoc();
        ~PopLoc();

        bool SetString(int theId, const std::wstring& theString, bool theOverwrite);
    };
}