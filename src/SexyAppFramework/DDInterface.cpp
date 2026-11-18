#include "DDInterface.h"

#include "AutoCrit.h"
#include "D3DInterface.h"

using namespace Sexy;

bool Sexy::gIs3D = false;

DDInterface::DDInterface(SexyAppBase *theApp)
{
    // TODO: Implement

    mD3DInterface = nullptr;
    mRenderDevice3D = nullptr;

    // TODO: Implement
}

DDInterface::~DDInterface()
{
    // TODO: Implement
}

void DDInterface::WindowResize(int width, int height)
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

int DDInterface::Init(HWND theWindow, bool IsWindowed)
{
    AutoCrit anAutoCrit(mCritSect);

    mInitialized = false;

    Cleanup();

    if (mD3DInterface != nullptr)
        delete mD3DInterface;

    mD3DInterface = nullptr;
    mRenderDevice3D = nullptr;

    // TODO: Implement

    return RESULT_OK;
}

bool DDInterface::SetCursorImage(Image *theImage)
{
    // TODO: Implement
    return false;
}

void DDInterface::Cleanup()
{
    // TODO: Implement
}
