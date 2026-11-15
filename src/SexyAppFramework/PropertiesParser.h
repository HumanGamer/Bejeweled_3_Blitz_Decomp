#pragma once

#include "Common.h"

namespace Sexy
{
    class XMLParser;
    class Buffer;
    class SexyAppBase;

    class PropertiesParser
    {
    public:
        SexyAppBase* mApp;
        XMLParser* mXMLParser;
        SexyString mError;
        bool mHasFailed;

    protected:
        // TODO: Implement

    public:
        PropertiesParser(SexyAppBase* theApp);
        virtual ~PropertiesParser();

        // TODO: Implement

        bool ParsePropertiesBuffer(const Buffer& theBuffer);
        SexyString GetErrorText();
    };
}