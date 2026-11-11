#include "PopLoc.h"

using namespace Sexy;

PopLoc::PopLoc()
{

}

PopLoc::~PopLoc()
{

}

bool PopLoc::SetString(int theId, const std::wstring &theString, bool theOverwrite)
{
    std::pair<IntWStringMap::iterator, bool> aResult =
        mIdStrings.insert(std::make_pair(theId, theString));

    if (!aResult.second && theOverwrite) // folded away in the binary, but likely was this
        aResult.first->second = theString;

    return aResult.second;
}
