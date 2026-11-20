#include "DDInterface.h"

#include "AutoCrit.h"
#include "D3DInterface.h"
#include "Debug.h"
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

    if (mDesktopAspect < mApp->mMinAspect)
        mDesktopAspect = mApp->mMinAspect;

    if (mDesktopAspect > mApp->mMaxAspect)
        mDesktopAspect = mApp->mMaxAspect;

    if (Do3DTest(theWindow))
    {
        if (mD3DTester->ResultsChanged())
            mApp->Done3dTesting();
    }

    RECT aRect;
    GetClientRect(theWindow, &aRect);

    mHWnd = theWindow;
    mWidth = mApp->mWidth;
    mHeight = mApp->mHeight;
    mAspect.Set(mWidth, mHeight);
    mDesktopWidth = GetSystemMetrics(SM_CXSCREEN);
    mDesktopHeight = GetSystemMetrics(SM_CYSCREEN);
    mDesktopAspect.Set(mDesktopWidth, mDesktopHeight);

    if (mDesktopAspect < mApp->mMinAspect)
        mDesktopAspect = mApp->mMinAspect;

    if (mDesktopAspect > mApp->mMaxAspect)
        mDesktopAspect = mApp->mMaxAspect;

    mDisplayWidth = mWidth;
    mDisplayHeight = mHeight;
    mDisplayAspect = mAspect;
    mPresentationRect = Rect(0, 0, mWidth, mHeight);
    mApp->mScreenBounds = mPresentationRect;
    mFullscreenBits = mApp->mFullscreenBits;
    mIsWindowed = isWindowed;
    mHasOldCursorArea = false;

    OutputDebug(_S("Application requests %4lu x %4lu [%2d:%2d]\n"), mWidth, mHeight, mAspect.mNumerator, mAspect.mDenominator);

    if (GotDXError(aResult, "DirectDrawCreate"))
        return RESULT_DD_CREATE_FAIL;

    if (isWindowed)
    {
        OutputDebug(_S("Hack aspect is                   [%2d:%2d]\n"), mApp->mWindowAspect.mNumerator, mApp->mWindowAspect.mDenominator);

        Ratio newAspect = mApp->mWindowAspect;

        if (newAspect < mApp->mMinAspect)
            newAspect = mApp->mMinAspect;

        if (newAspect > mApp->mMaxAspect)
            newAspect = mApp->mMaxAspect;

        if (mApp->mEnableWindowAspect && mHeight * newAspect != mWidth)
        {
            mIsWidescreen = true;

            // Setup the window at the hack aspect,
            // but with the height requested by the application.
            mDisplayWidth = mHeight * mApp->mWindowAspect;
            mDisplayHeight = mHeight;
            mDisplayAspect = mApp->mWindowAspect;

            if (newAspect != mApp->mWindowAspect)
            {
                // resize the window.
                RECT rc;
                POINT pt;
                WINDOWINFO info;
                ::GetWindowInfo(theWindow, &info);
                ::GetClientRect(theWindow, &rc);
                pt.x = rc.left;
                pt.y = rc.top;
                ::ClientToScreen(theWindow, &pt);
                rc.left = pt.x - (mDisplayWidth - mWidth) / 2;
                rc.top = pt.y - (mDisplayHeight - mHeight) / 2;
                rc.right = rc.left + mDisplayWidth;
                rc.bottom = rc.top + mDisplayHeight;
                ::AdjustWindowRectEx(&rc, info.dwStyle, false, info.dwExStyle);
                ::MoveWindow(theWindow, max(0, rc.left), max(0, rc.top), rc.right-rc.left, rc.bottom-rc.top, false);
            }

            if (mApp->mWidescreenAware)
            {
                mWidth = mDisplayWidth;
                mHeight = mDisplayHeight;
                mAspect = mDisplayAspect;

                mPresentationRect.mWidth = mDisplayWidth;
                mPresentationRect.mHeight = mDisplayHeight;
                mPresentationRect.mX = 0;
                mPresentationRect.mY = 0;
            }
            else
            {
                // Set the dest rect for drawing the back buffer to the center of
                // the wide display.
                mPresentationRect.mWidth = mWidth;
                mPresentationRect.mHeight = mHeight;
                mPresentationRect.mX = (mDisplayWidth - mPresentationRect.mWidth) / 2;
                mPresentationRect.mY = 0;
            }
        }

        OutputDebug(_S("Window is            %4lu x %4lu [%2d:%2d]\n"), mDisplayWidth, mDisplayHeight, mDisplayAspect.mNumerator, mDisplayAspect.mDenominator);
    }
    else
    {
        OutputDebug(_S("Desktop is           %4lu x %4lu [%2d:%2d]\n"), mDesktopWidth, mDesktopHeight, mDesktopAspect.mNumerator, mDesktopAspect.mDenominator);

        if (mIs3D && (mAspect != mDesktopAspect || mDisplayWidth != mDesktopWidth || mDisplayHeight != mDesktopHeight))
        {
            mIsWidescreen = true;

            // Set the display mode to the size of the desktop.
            mDisplayWidth = mDesktopWidth;
            mDisplayHeight = mDesktopHeight;
            mDisplayAspect = mDesktopAspect;

            if (mApp->mWidescreenAware)
            {
                if (mDisplayAspect > mAspect)
                    mDisplayAspect = mAspect;

                // Setup the draw buffer(s) at the same aspect ratio as the desktop,
                // but with the height requested by the application.
                mAspect = mDisplayAspect;
                mWidth = mHeight * mAspect;

                mPresentationRect.mWidth = mDisplayWidth;
                mPresentationRect.mHeight = mDisplayHeight;
                mPresentationRect.mX = 0;
                mPresentationRect.mY = 0;
            }
            else
            {
                // Set the dest rect for drawing the back buffer to the center of
                // the wide display.
                mPresentationRect.mWidth = mWidth * mDisplayHeight / mHeight;
                mPresentationRect.mHeight = mDisplayHeight;
                mPresentationRect.mX = (mDisplayWidth - mPresentationRect.mWidth) / 2;
                mPresentationRect.mY = 0;
            }
        }

        OutputDebug(_S("Display is           %4lu x %4lu [%2d:%2d]\n"), mDisplayWidth, mDisplayHeight, mDisplayAspect.mNumerator, mDisplayAspect.mDenominator);
    }

    mIsD3D9 = false;
    mIsD3D8 = false;
    mIsD3D8Or9 = false;
    if (mIs3D)
    {
        mD3DInterface = nullptr;
        mRenderDevice3D = nullptr;
        if (mWantD3D9)
        {
            mD3DInterface = CreateD3D9Interface();
            mRenderDevice3D = mD3DInterface;

            EResult result = RESULT_OK;
            if (!mD3DInterface->InitFromDDInterface(this, &result, false, nullptr))
            {
                if (mD3DInterface != nullptr)
                    delete mD3DInterface;

                mD3DInterface = nullptr;
                mRenderDevice3D = nullptr;

                if (result != RESULT_OK)
                    return result;
            }
        }

        if (mD3DInterface == nullptr)
        {
            mD3DInterface = CreateD3D8Interface();
            mRenderDevice3D = mD3DInterface;

            EResult result = RESULT_OK;
            if (!mD3DInterface->InitFromDDInterface(this, &result, false, nullptr))
            {
                if (mD3DInterface != nullptr)
                    delete mD3DInterface;

                mD3DInterface = nullptr;
                mRenderDevice3D = nullptr;

                if (result != RESULT_OK)
                    return result;
            }
        }

        if (mD3DInterface == nullptr)
        {
            return RESULT_3D_FAIL;
        }

        if (!isWindowed)
        {
            ulong width;
            ulong height;
            mD3DInterface->GetBackBufferDimensions(&width, &height);

            if (width != mDisplayWidth || height != mDisplayHeight)
            {
                mDisplayWidth = width;
                mDisplayHeight = height;
                mDisplayAspect.Set(mDisplayWidth, mDisplayHeight);

                if (mApp->mWidescreenAware)
                {
                    if (mDisplayAspect > mAspect)
                        mAspect = mDisplayAspect;

                    mWidth = mHeight * mAspect;
                    mPresentationRect.mWidth = mDisplayWidth;
                    mPresentationRect.mHeight = mDisplayHeight;
                    mPresentationRect.mX = 0;
                    mPresentationRect.mY = 0;
                }
                else
                {
                    mPresentationRect.mWidth = mDisplayHeight * mWidth / mHeight;
                    mPresentationRect.mHeight = mDisplayHeight;
                    mPresentationRect.mX = (mDisplayWidth - mPresentationRect.mWidth) / 2;
                    mPresentationRect.mY = 0;
                }
            }
        }

        if (mDD != nullptr)
        {
            mDD->SetCooperativeLevel(mHWnd, DDSCL_NORMAL);
            mDD->Release();
            mDD = nullptr;
        }

        if (mDD7 != nullptr)
        {
            mDD7->Release();
            mDD7 = nullptr;
        }

        mInitialized = true;
    }

    if (!mIsD3D8Or9)
    {
        // TODO: Implement
    }

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

bool DDInterface::Do3DTest(HWND theHWND)
{
    if (mD3DTester == nullptr)
    {
        if (mApp->mTest3D || mApp->mAutoEnable3D)
        {
            mD3DTester = new D3DTester;
            mD3DTester->SetVidMemoryConstraints(mApp->mMinVidMemory3D, mApp->mRecommendedVidMemory3D);
            mD3DTester->TestD3D(theHWND, mDD7);

            if (mApp->mAutoEnable3D && mD3DTester->Is3DRecommended())
                mIs3D = true;

            if (!mD3DTester->Is3DSupported())
                mIs3D = false;

            if (mD3DTester->ResultsChanged() && !mD3DTester->Is3DRecommended())
                mIs3D = false;

            return true;
        }
    }

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
