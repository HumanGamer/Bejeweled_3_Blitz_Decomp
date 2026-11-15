#include "PropertiesParser.h"

using namespace Sexy;

PropertiesParser::PropertiesParser(SexyAppBase *theApp)
{
    mApp = theApp;
    mHasFailed = false;
    mXMLParser = nullptr;
}

PropertiesParser::~PropertiesParser() = default;

bool PropertiesParser::ParsePropertiesBuffer(const Buffer &theBuffer)
{
    // TODO: Implement
    return false;
}

SexyString PropertiesParser::GetErrorText()
{
    return mError;
}
