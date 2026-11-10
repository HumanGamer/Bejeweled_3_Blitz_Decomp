#include "Common.h"

HINSTANCE Sexy::gHInstance;

std::string Sexy::GetFileDir(const std::string& thePath, bool withSlash)
{
    int aLastSlash = max((int) thePath.rfind('\\'), (int) thePath.rfind('/'));

    if (aLastSlash == -1)
        return "";
    else
    {
        if (withSlash)
            return thePath.substr(0, aLastSlash+1);
        else
            return thePath.substr(0, aLastSlash);
    }
}