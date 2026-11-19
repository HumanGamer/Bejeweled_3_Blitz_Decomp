#include "DDInterface.h"

#include "AutoCrit.h"
#include "D3DInterface.h"
#include "DirectXErrorString.h"
#include "SexyAppBase.h"

using namespace Sexy;
typedef HRESULT (WINAPI *DirectDrawCreateFunc)(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
typedef HRESULT (WINAPI *DirectDrawCreateExFunc)(GUID FAR *lpGUID, LPVOID *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter);

extern HMODULE gDDrawDLL;
static DirectDrawCreateFunc gDirectDrawCreateFunc = nullptr;
static DirectDrawCreateExFunc gDirectDrawCreateExFunc = nullptr;

bool Sexy::gIs3D = false;

DDInterface::DDInterface(SexyAppBase *theApp)
{
    mApp = theApp;
    mPrimarySurface = nullptr;
    mDrawSurface = nullptr;
    mSecondarySurface = nullptr;
    mScreenImage = nullptr;
    mSecondarySurfaceImage = nullptr;
    mDD = nullptr;
    mDD7 = nullptr;
    mRedAddTable = nullptr;
    mGreenAddTable = nullptr;
    mBlueAddTable = nullptr;
    mInitialized = false;
    mScanLineFailCount = 0;
    mNextCursorX = 0;
    mNextCursorY = 0;
    mCursorX = 0;
    mCursorY = 0;
    mInRedraw = false;
    mInRedrawCursor = false;
    mCursorWidth = 64;
    mCursorHeight = 64;
    mCursorImage = nullptr;
    mHasOldCursorArea = false;
    mOldCursorArea = nullptr;
    mNewCursorArea = nullptr;
    mNewCursorAreaImage = nullptr;
    mOldCursorAreaImage = nullptr;
    mInitCount = 0;
    mRefreshRate = 60;
    mMillisecondsPerFrame = 16;
    mD3DInterface = nullptr;
    mRenderDevice3D = nullptr;

    for (int i = 0; i < 4; i++)
        mWindowScaleBuffers[i] = nullptr;

    mIs3D = false;
    mFov = 0.7853982f;
    mNearPlane = 0.1f;
    mFarPlane = 100.0f;
    mWantD3D9 = !mApp->mNoD3D9;
    mIsD3D9 = false;
    mIsD3D8 = false;
    mIsD3D8Or9 = false;
    mPrimaryDXSurface = nullptr;

    mD3DTester = nullptr;

    gDirectDrawCreateFunc = (DirectDrawCreateFunc)GetProcAddress(gDDrawDLL, "DirectDrawCreate");
    gDirectDrawCreateExFunc = (DirectDrawCreateExFunc)GetProcAddress(gDDrawDLL, "DirectDrawCreateEx");

    mRenderMode = RENDERMODE_Invalid;
    mRenderModeFlags = RENDERMODEF_NoStretchRectFromTextures | RENDERMODEF_PreventLag;

}

DDInterface::~DDInterface()
{
    delete[] mRedAddTable;
    delete[] mGreenAddTable;
    delete[] mBlueAddTable;

    Cleanup();
    Cleanup3DObjects();

    delete mD3DInterface;
    delete mD3DTester;

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

int DDInterface::Init(HWND theWindow, bool isWindowed)
{
    AutoCrit anAutoCrit(mCritSect);

    mInitialized = false;

    Cleanup();

    if (mD3DInterface != nullptr)
        delete mD3DInterface;

    mD3DInterface = nullptr;
    mRenderDevice3D = nullptr;

    mApp->SetInteger("compat_D3DInterface", 0);

    HRESULT aResult;
    if (gDirectDrawCreateExFunc != nullptr)
    {
        aResult = gDirectDrawCreateExFunc(nullptr, (LPVOID*) &mDD7, IID_IDirectDraw7, nullptr);
        if (GotDXError(aResult, "DirectDrawCreateEx"))
            return RESULT_DD_CREATE_FAIL;

        aResult = mDD7->QueryInterface(IID_IDirectDraw, (LPVOID*) &mDD);
        if (GotDXError(aResult, "QueryInterface IID_IDirectDraw"))
            return RESULT_DD_CREATE_FAIL;
    }
    else
    {
        aResult = gDirectDrawCreateFunc(nullptr, &mDD, nullptr);
    }

    mHWnd = theWindow;
    mWidth = mApp->mWidth;
    mHeight = mApp->mHeight;
    mFullscreenBits = mApp->mFullscreenBits;
    mIsWindowed = isWindowed;
    mDesktopWidth = GetSystemMetrics(SM_CXSCREEN);
    mDesktopHeight = GetSystemMetrics(SM_CYSCREEN);
    mDesktopAspect.Set(mDesktopWidth, mDesktopHeight);

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

void DDInterface::Cleanup3DObjects()
{
    // TODO: Implement
}

bool DDInterface::GotDXError(HRESULT theResult, const char *theContext)
{
    if (!SUCCEEDED(theResult))
    {
        std::string anError = GetDirectXErrorString(theResult);
        mErrorString = StrFormat("%s: %s",theContext, anError.c_str());

        return true;
    }
    else
        return false;
}

IDXSurface::IDXSurface()
{
    // TODO: Implement
}

IDXSurface::~IDXSurface()
{
    // TODO: Implement
}
