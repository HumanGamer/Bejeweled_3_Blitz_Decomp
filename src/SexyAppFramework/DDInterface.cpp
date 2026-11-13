#include "DDInterface.h"

using namespace Sexy;

DDInterface::DDInterface(SexyAppBase *theApp)
{
    // TODO: Implement
}

DDInterface::~DDInterface()
{
    // TODO: Implement
}

std::string DDInterface::ResultToString(int theResult)
{
    switch (theResult)
    {
        case RESULT_OK:
            return "RESULT_OK";
        case RESULT_FAIL:
            return "RESULT_FAIL";
        case RESULT_DD_CREATE_FAIL:
            return "RESULT_DD_CREATE_FAIL";
        case RESULT_SURFACE_FAIL:
            return "RESULT_SURFACE_FAIL";
        case RESULT_EXCLUSIVE_FAIL:
            return "RESULT_EXCLUSIVE_FAIL";
        case RESULT_DISPCHANGE_FAIL:
            return "RESULT_DISPCHANGE_FAIL";
        case RESULT_INVALID_COLORDEPTH:
            return "RESULT_INVALID_COLORDEPTH";

        // This was missing in the decomp and SexyAppFramework 1.3
        case RESULT_3D_FAIL:
            return "RESULT_3D_FAIL";

        // This was missing in the decomp (and it didn't exist in SexyAppFramework 1.3)
        case RESULT_3D_NOTREADY:
            return "RESULT_3D_NOTREADY";

        default:
            return "RESULT_UNKNOWN";
    }
}

DDImage *DDInterface::GetScreenImage()
{
    return mScreenImage;
}
