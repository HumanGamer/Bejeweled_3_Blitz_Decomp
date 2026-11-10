#pragma once

#include "Common.h"

namespace Sexy
{
    typedef std::map<int, std::wstring> IntWStringMap;
    class PopLoc
    {
    public:
        IntWStringMap mIdStrings;
        WStringWStringMap mNameStrings;

    public:
        PopLoc();
        ~PopLoc();

        bool SetString(int id, const std::wstring& str, bool unknown);
    };
}