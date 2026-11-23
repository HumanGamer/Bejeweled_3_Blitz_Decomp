#include "DDInterface.h"

#include "AutoCrit.h"
#include "CfgMachine.h"
#include "D3D8Interface.h"
#include "D3D9Interface.h"
#include "D3DInterface.h"
#include "DDImage.h"
#include "Debug.h"
#include "DirectXErrorString.h"
#include "DXSurface7.h"
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

    OutputDebugStrF("Application requests %4lu x %4lu [%2d:%2d]\n", mWidth, mHeight, mAspect.mNumerator, mAspect.mDenominator);

    if (GotDXError(aResult, "DirectDrawCreate"))
        return RESULT_DD_CREATE_FAIL;

    if (isWindowed)
    {
        OutputDebugStrF("Hack aspect is                   [%2d:%2d]\n", mApp->mWindowAspect.mNumerator, mApp->mWindowAspect.mDenominator);

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

        OutputDebugStrF("Window is            %4lu x %4lu [%2d:%2d]\n", mDisplayWidth, mDisplayHeight, mDisplayAspect.mNumerator, mDisplayAspect.mDenominator);
    }
    else
    {
        OutputDebugStrF("Desktop is           %4lu x %4lu [%2d:%2d]\n", mDesktopWidth, mDesktopHeight, mDesktopAspect.mNumerator, mDesktopAspect.mDenominator);

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

        OutputDebugStrF("Display is           %4lu x %4lu [%2d:%2d]\n", mDisplayWidth, mDisplayHeight, mDisplayAspect.mNumerator, mDisplayAspect.mDenominator);
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
            mD3DInterface->GetBackBufferDimensions(width, height);

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
        if (isWindowed)
        {
            mDD->SetCooperativeLevel(mHWnd, DDSCL_NORMAL);

            DDSURFACEDESC2 ddsd;
            ZeroMemory(&ddsd, sizeof(ddsd));
            ddsd.dwSize = sizeof(ddsd);
            ddsd.dwFlags = DDSD_CAPS;
            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
            if (mIs3D)
                ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
            ddsd.dwBackBufferCount = 1;
            HRESULT result = CreateSurface(&ddsd, &mPrimarySurface, nullptr);
            if (result == DDERR_INVALIDPIXELFORMAT)
                return RESULT_INVALID_COLORDEPTH;
            if (GotDXError(result, "CreateSurface Windowed Primary"))
                return RESULT_SURFACE_FAIL;

            mPrimaryDXSurface = new DXSurface7(this);
            mPrimaryDXSurface->SetSurface(mPrimarySurface);

            DDSURFACEDESC2 ddsd2;
            ZeroMemory(&ddsd2, sizeof(ddsd2));
            ddsd2.dwSize = sizeof(ddsd2);
            ddsd2.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
            ddsd2.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
            if (mIs3D)
                ddsd2.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;
            ddsd2.dwWidth = mWidth;
            ddsd2.dwHeight = mHeight;
            result = CreateSurface(&ddsd2, &mDrawSurface, nullptr);
            if (GotDXError(result, "CreateSurface Windowed DrawSurface"))
                return RESULT_SURFACE_FAIL;

            IDirectDrawClipper* aClipper;
            result = mDD->CreateClipper(0, &aClipper, nullptr);
            if (GotDXError(aResult,"CreateClipper Windowed"))
                return RESULT_FAIL;

            result = aClipper->SetHWnd(0, theWindow);
            if (SUCCEEDED(result))
                result = mPrimarySurface->SetClipper(aClipper);

            aClipper->Release();
        }
        else
        {
            HRESULT result = mDD->SetCooperativeLevel(mHWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT | DDSCL_NOWINDOWCHANGES);
            if (GotDXError(result, "SetCooperativeLevel FullScreen"))
                return RESULT_EXCLUSIVE_FAIL;

            result = mDD->SetDisplayMode(mDisplayWidth, mDisplayHeight, mFullscreenBits);
            if (GotDXError(result, "SetDisplayMode FullScreen"))
                return RESULT_DISPCHANGE_FAIL;

            DDSURFACEDESC2 ddsd;
            ZeroMemory(&ddsd, sizeof(ddsd));
            ddsd.dwSize = sizeof(ddsd);
            ddsd.dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
            if (mIs3D)
                ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
            result = CreateSurface(&ddsd, &mPrimarySurface, nullptr);
            if (GotDXError(result, "CreateSurface FullScreen Primary"))
                return RESULT_SURFACE_FAIL;

            DDSCAPS aCaps;
            ZeroMemory(&aCaps, sizeof(aCaps));
            aCaps.dwCaps = DDSCAPS_BACKBUFFER;
            result = mPrimarySurface->GetAttachedSurface(&aCaps, &mSecondarySurface);
            if (GotDXError(result, "GetAttachedSurface"))
                return RESULT_SURFACE_FAIL;

            if (mIsWidescreen)
            {
                ClearSurface(mPrimarySurface);
                ClearSurface(mSecondarySurface);
            }

            DDSURFACEDESC2 ddsd2;
            ZeroMemory(&ddsd2, sizeof(ddsd2));
            ddsd2.dwSize = sizeof(ddsd2);
            ddsd2.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
            ddsd2.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
            if (mIs3D)
                ddsd2.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;
            ddsd2.dwWidth = mWidth;
            ddsd2.dwHeight = mHeight;
            result = CreateSurface(&ddsd2, &mDrawSurface, nullptr);
            if (GotDXError(result, "CreateSurface FullScreen DrawSurface"))
                return RESULT_SURFACE_FAIL;

            IDirectDrawClipper* aClipper;
            result = mDD->CreateClipper(0, &aClipper, nullptr);
            if (GotDXError(aResult,"CreateClipper FullScreen"))
                return RESULT_FAIL;

            result = aClipper->SetHWnd(0, theWindow);
            if (SUCCEEDED(result))
                result = mPrimarySurface->SetClipper(aClipper);

            mSecondarySurfaceImage = new DDImage(this);
            mSecondarySurfaceImage->SetSurface(mSecondarySurface);
            if (mIsWidescreen)
            {
                ClearSurface(mPrimarySurface);
                ClearSurface(mSecondarySurface);
            }

            aClipper->Release();

            if (!mApp->mFullScreenPageFlip)
                mDD->FlipToGDISurface();
        }

        if (mDD->GetMonitorFrequency(&mRefreshRate) < 0 || mRefreshRate < 60)
        {
            mApp->mVSyncBroken = true;
            mRefreshRate = 60;
        }
    }

    RECT rect;
    GetClientRect(mHWnd, &rect);
    OutputDebugStrF("mHWnd=%08X mW=%d mH=%d WW=%d WH=%d Window=%d D3D8=%d D3D9=%d\r\n", mHWnd, mWidth, mHeight, aRect.right, aRect.bottom, mIsWindowed, mIsD3D8, mIsD3D9);

    if ((mIsWindowed || mIsD3D8Or9) && (rect.right != mWidth || rect.bottom != mHeight))
        WindowResize(rect.right, rect.bottom);

    if (mRefreshRate < 60)
    {
        mApp->mVSyncBroken = true;
        mRefreshRate = 60;
    }
    else if (mRefreshRate > 100) // We must have at least 1 Update per UpdateF for demo compatibility
    {
        mApp->mVSyncBroken = true;
        mRefreshRate = 100;
    }

    OutputDebugStrF("Draw buffer is       %4lu x %4lu [%2d:%2d]\n", mWidth, mHeight, mAspect.mNumerator, mAspect.mDenominator);

    mMillisecondsPerFrame = 1000 / mRefreshRate;

    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = mCursorWidth;
    ddsd.dwHeight = mCursorHeight;

    if (!mIsD3D8Or9)
    {
        HRESULT result = CreateSurface(&ddsd, &mOldCursorArea, nullptr);
        if (GotDXError(result, "CreateSurface OldCursorArea"))
            return RESULT_SURFACE_FAIL;

        mOldCursorAreaImage = new DDImage(this);
        mOldCursorAreaImage->SetSurface(mOldCursorArea);
        mOldCursorAreaImage->SetImageMode(false, false);

        result = CreateSurface(&ddsd, &mNewCursorArea, nullptr);
        if (GotDXError(result, "CreateSurface NewCursorArea"))
            return RESULT_SURFACE_FAIL;

        mNewCursorAreaImage = new DDImage(this);
        mNewCursorAreaImage->SetSurface(mNewCursorArea);
        mNewCursorAreaImage->SetImageMode(false, false);
    }

    if (mPrimarySurface != nullptr)
    {
        DDSURFACEDESC ddsd2;
        ZeroMemory(&ddsd2, sizeof(ddsd2));
        ddsd2.dwSize = sizeof(ddsd2);
        ddsd2.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
        mPrimarySurface->GetSurfaceDesc(&ddsd2);
        if (ddsd2.ddpfPixelFormat.dwRGBBitCount != 16 && ddsd2.ddpfPixelFormat.dwRGBBitCount != 32)
            return RESULT_INVALID_COLORDEPTH;

        mRGBBits = ddsd2.ddpfPixelFormat.dwRGBBitCount;
        mRedMask = ddsd2.ddpfPixelFormat.dwRBitMask;
        mGreenMask = ddsd2.ddpfPixelFormat.dwGBitMask;
        mBlueMask = ddsd2.ddpfPixelFormat.dwBBitMask;

        for (int i = 32; i >= 0; i--)
        {
            if (((mRedMask >> i) & 1) != 0)
                mRedShift = i;
            if (((mGreenMask >> i) & 1) != 0)
                mGreenShift = i;
            if (((mBlueMask >> i) & 1) != 0)
                mBlueShift = i;
        }

        for (int i = 0; i < 32; i++)
        {
            if ((i+mRedShift < 32) && ((mRedMask >> (i+mRedShift)) != 0))
                mRedBits = i+1;
            if ((i+mGreenShift < 32) && ((mGreenMask >> (i+mGreenShift)) != 0))
                mGreenBits = i+1;
            if ((i+mBlueShift < 32) && ((mBlueMask >> (i+mBlueShift)) != 0))
                mBlueBits = i+1;
        }

        delete [] mRedAddTable;
        delete [] mGreenAddTable;
        delete [] mBlueAddTable;

        int aMaxR = (1<<mRedBits) - 1;
        int aMaxG = (1<<mGreenBits) - 1;
        int aMaxB = (1<<mBlueBits) - 1;

        mRedAddTable = new int[aMaxR*2+1];
        mGreenAddTable = new int[aMaxG*2+1];
        mBlueAddTable = new int[aMaxB*2+1];

        for (int i = 0; i < aMaxR*2+1; i++)
            mRedAddTable[i] = min(i, aMaxR);
        for (int i = 0; i < aMaxG*2+1; i++)
            mGreenAddTable[i] = min(i, aMaxG);
        for (int i = 0; i < aMaxB*2+1; i++)
            mBlueAddTable[i] = min(i, aMaxB);

        // Create the tables that we will use to convert from
        // internal color representation to surface representation
        for (int i = 0; i < 256; i++)
        {
            mRedConvTable[i] = ((i * mRedMask) / 255) & mRedMask;
            mGreenConvTable[i] = ((i * mGreenMask) / 255) & mGreenMask;
            mBlueConvTable[i] = ((i * mBlueMask) / 255) & mBlueMask;
        }
    }

    if (mIs3D)
    {
        if (!mIsD3D8Or9 && !mD3DInterface->InitFromDDInterface(this, nullptr, false, nullptr))
        {
            mErrorString = "3D init error: " + D3DInterface::sErrorString;
            return RESULT_3D_FAIL;
        }
    }
    else
    {
        mScreenImage = new DDImage(this);
        mScreenImage->SetSurface(mDrawSurface);
        mScreenImage->SetImageMode(false, false);
    }

    mInitCount++;
    mInitialized = true;

    if (mApp->mCompatCfgMachine != nullptr)
        mApp->mCompatCfgMachine->MachineExecuteFunction("DDInterfaceReady", nullptr);

    return RESULT_OK;
}

bool DDInterface::Redraw(Rect *theClipRect)
{
    AutoCrit anAutoCrit(mCritSect);

    if (!mInitialized)
        return false;

    DDBLTFX aBltFX;
    ZeroMemory(&aBltFX, sizeof(aBltFX));
    aBltFX.dwSize = sizeof(aBltFX);

    mInRedraw = true;

    GetTickCount();
    if (mIs3D)
    {
        for (D3DObjectSet::iterator anItr = mD3DObjectSet.begin(); anItr != mD3DObjectSet.end(); ++anItr)
        {
            D3DObject* anObject = *anItr;
            if (anObject->mFrame == nullptr)
                return false;
        }

        if (!D3DInterface::sErrorString.empty())
        {
            OutputDebugStringA(D3DInterface::sErrorString.c_str());
            OutputDebugStringA("\r\n");

            mInRedraw = false;
            mIs3D = false;

            return false;
        }

        mD3DInterface->Flush(2); // TODO: Figure out flags instead of using raw number
    }

    RECT aDestRect;
    RECT aSrcRect;
    if (theClipRect == nullptr || mIsWidescreen)
    {
        // ClipRect cannot be supported when the draw surface and
        // primary surface are not the same size in widescreen mode.
        aDestRect = mPresentationRect.ToRECT();
        aSrcRect = Rect(0, 0, mWidth, mHeight).ToRECT();
    }
    else
    {
        aDestRect = theClipRect->ToRECT();
        aSrcRect = theClipRect->ToRECT();
    }

    if ( mIsWidescreen )
    {
        aBltFX.dwDDFX = DDBLTFX_ARITHSTRETCHY;
    }

    if (mIsD3D8Or9)
    {
        if (mWindowScaleBuffers[0] != nullptr)
        {
            int iter = 0;
            do
            {
                if (*mWindowScaleBuffers == nullptr)
                    break;

                if (iter != 3 && mWindowScaleBuffers[1] != nullptr || mIsD3D8Or9)
                {
                    // TODO: Implement
                }
                else
                {
                    // TODO: Implement
                }

                // TODO: Implement

                iter++;
            } while (iter < 4);

            // TODO: Implement
        }

        // TODO: Implement
    }

    POINT aPoint = {0, 0};

    ClientToScreen(mHWnd, &aPoint);
    OffsetRect(&aDestRect, aPoint.x, aPoint.y);

    DDSURFACEDESC aDesc;
    ZeroMemory(&aDesc, sizeof(aDesc));
    aDesc.dwSize = sizeof(aDesc);
    mDrawSurface->Lock(nullptr, &aDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_READONLY, nullptr);
    mDrawSurface->Unlock(nullptr);

    HRESULT aResult;
    if (!mIsWindowed)
    {
        if (mApp->mNoDefer || !mApp->mFullScreenPageFlip)
        {
            aResult = mPrimarySurface->Blt(&aDestRect, mDrawSurface, &aSrcRect, DDBLT_WAIT, &aBltFX);
            mInRedraw = false;
            return !GotDXError(aResult, "Redraw FullScreen Blt");
        }
        else
        {
            aResult = mSecondarySurface->Blt(&aDestRect, mDrawSurface, &aSrcRect, DDBLT_WAIT, &aBltFX);
            if (SUCCEEDED(aResult))
            {
                if (mIs3D && mIsWidescreen)
                {
                    Rect rect1;
                    rect1.mX = 0;
                    rect1.mY = 0;
                    rect1.mWidth = aDestRect.left;
                    rect1.mHeight = mDisplayHeight;

                    Rect rect2;
                    rect2.mX = aDestRect.right;
                    rect2.mY = 0;
                    rect2.mWidth = mDisplayWidth - aDestRect.right;
                    rect2.mHeight = mDisplayHeight;

                    if (aDestRect.left > 0)
                    {
                        mSecondarySurfaceImage->NormalFillRect(rect1, Color::White);
                    }
                    if (mDisplayWidth - aDestRect.right > 0)
                    {
                        mSecondarySurfaceImage->NormalFillRect(rect2, Color::White);
                    }
                }

                mCursorX = mNextCursorX;
                mCursorY = mNextCursorY;

                DrawCursorTo(mSecondarySurface, true);

                DWORD flags = DDFLIP_WAIT;
                if (mApp->mNoVSync)
                    flags |= DDFLIP_NOVSYNC;

                mPrimarySurface->Flip(nullptr, flags);

                mInRedraw = false;
                return !GotDXError(aResult, "Redraw FullScreen Flip (2)");
            }
            else
            {
                mInRedraw = false;
                return !GotDXError(aResult, "Redraw FullScreen Flip (1)");
            }
        }
    }

    //DWORD aScanLine;
    //mDD->GetScanLine(&aScanLine);

    int aScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (mWindowScaleBuffers[0] != nullptr)
    {
        LPDIRECTDRAWSURFACE** buffers = (LPDIRECTDRAWSURFACE**)mWindowScaleBuffers;

        LPDIRECTDRAWSURFACE theSurface = mDrawSurface;

        RECT dRect;
        RECT sRect;

        int iters = 0;
        do
        {
            if (*buffers == nullptr)
                break;

            if (iters == 3 || buffers[1] == nullptr)
            {
                dRect.left = mPresentationRect.mX;
                dRect.top = mPresentationRect.mY;
                dRect.right = mPresentationRect.mX + mPresentationRect.mWidth;
                dRect.bottom = mPresentationRect.mY + mPresentationRect.mHeight;
            }
            else
            {
                dRect.left = 0;
                dRect.top = 0;
                dRect.right = aSrcRect.right / 2;
                dRect.bottom = aSrcRect.bottom / 2;
            }

            LPDIRECTDRAWSURFACE surf = (LPDIRECTDRAWSURFACE)*buffers;
            surf->Blt(&dRect, theSurface, &sRect, DDBLT_WAIT, &aBltFX);

            mDrawSurface = (LPDIRECTDRAWSURFACE)*buffers;

            ++iters;
            ++buffers;
            aSrcRect = dRect; // TODO: Is this correct?
        } while (iters < 4);
    }

    mCursorX = mNextCursorX;
    mCursorY = mNextCursorY;

    DrawCursorTo(mDrawSurface, false);

    if (!mApp->mWaitForVSync || mApp->mSoftVSyncWait)
    {
LABEL_82:
        aResult = mPrimarySurface->Blt(&aDestRect, mDrawSurface, &aSrcRect, DDBLT_WAIT, &aBltFX);
        goto LABEL_83;
    }

    if (mScanLineFailCount >= 3)
    {
LABEL_81:
        mDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
        goto LABEL_82;
    }

    int v59 = mApp->mHeight / 2;
    int v93 = aDestRect.top + v59;
    int v87x = aDestRect.top + mHeight;
    bool v60 = false;
    int ticks = GetTickCount();
    DWORD scanLine = ~0x80000000;

    while (true)
    {
        aResult = mDD->GetScanLine(&scanLine);
        if (aResult != DD_OK)
            break;
        if (v93 - scanLine <= 0 || scanLine > aScreenHeight)
            goto LABEL_78;
        v60 = true;
LABEL_79:
        if (GetTickCount() - ticks >= 200)
        {
LABEL_80:
            mScanLineFailCount++;
            goto LABEL_81;
        }
    }

    if (aResult != DDERR_VERTICALBLANKINPROGRESS)
        goto LABEL_80;
LABEL_78:
    if (!v60)
        goto LABEL_79;

    RECT rDst;
    rDst.left = aDestRect.left;
    rDst.bottom = v93;
    rDst.top = aDestRect.top;
    rDst.right = aDestRect.right;

    RECT rSrc;
    rSrc.right = aSrcRect.right;
    rSrc.left = aSrcRect.left;
    rSrc.top = aSrcRect.top;
    rSrc.bottom = v59;

    mScanLineFailCount = 0;

    mPrimarySurface->Blt(&rDst, mDrawSurface, &rSrc, DDBLT_WAIT, &aBltFX);

    DWORD maxScanLine = scanLine;
    do
    {
        while (mDD->GetScanLine(&scanLine) < 0);
    } while (scanLine >= maxScanLine && v87x - scanLine > 0);

    rDst.left = aDestRect.left;
    rDst.bottom = aDestRect.bottom;
    rDst.top = v93;
    rDst.right = aDestRect.right;

    rSrc.right = aSrcRect.right;
    rSrc.left = aSrcRect.left;
    rSrc.top = v59;
    rSrc.bottom = aSrcRect.bottom;

    mPrimarySurface->Blt(&rDst, mDrawSurface, &rSrc, DDBLT_WAIT, &aBltFX);

LABEL_83:
    if (mHasOldCursorArea)
    {
        RestoreOldCursorAreaFrom(mDrawSurface, false);
        mHasOldCursorArea = true;
    }

    mInRedraw = false;

    return GotDXError(aResult, "Redraw Windowed");
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

void DDInterface::RestoreOldCursorAreaFrom(LPDIRECTDRAWSURFACE theSurface, bool adjust)
{
    // TODO: Implement
}

void DDInterface::DrawCursorTo(LPDIRECTDRAWSURFACE theSurface, bool adjust)
{
    // TODO: Implement
}

HRESULT DDInterface::CreateSurface(DDSURFACEDESC2 *theDesc, LPDIRECTDRAWSURFACE *theSurface, void* unused)
{
    if (mIsD3D8Or9)
    {
        if (theDesc != nullptr)
        {
            if ((theDesc->dwFlags & DDSD_WIDTH | DDSD_HEIGHT) != (DDSD_WIDTH | DDSD_HEIGHT))
            {
#ifdef OPTIMIZE_SOFTWARE_DRAWING
                gOptimizeSoftwareDrawing = true;
#endif
                return DDERR_INVALIDCAPS;
            }

            DWORD flags = 0;
            if ((theDesc->dwFlags & DDSD_CAPS) != 0)
                flags = (theDesc->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

            if (theSurface != nullptr)
            {
                *theSurface = (IDirectDrawSurface*)mD3DInterface->CreateSurface(theDesc->dwWidth, theDesc->dwHeight, flags, 0);

#ifdef OPTIMIZE_SOFTWARE_DRAWING
                gOptimizeSoftwareDrawing = true;
#endif

                return theSurface != nullptr ? DD_OK : DDERR_GENERIC;
            }
        }

#ifdef OPTIMIZE_SOFTWARE_DRAWING
        gOptimizeSoftwareDrawing = true;
#endif
        return DDERR_INVALIDPARAMS;
    }

    HRESULT res;
    if (mDD7 != nullptr)
    {
        LPDIRECTDRAWSURFACE7 surf;
        res = mDD7->CreateSurface(theDesc, &surf, nullptr);
        if (!SUCCEEDED(res))
            return res;

        res = surf->QueryInterface(IID_IDirectDrawSurface, (LPVOID*)theSurface);
        surf->Release();

        if (!SUCCEEDED(res))
            return res;
    }
    else
    {
        DDSURFACEDESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.dwSize = sizeof(desc);
        desc.dwFlags = theDesc->dwFlags;
        desc.lPitch = theDesc->lPitch;
        desc.dwWidth = theDesc->dwWidth;
        desc.dwHeight = theDesc->dwHeight;
        desc.dwAlphaBitDepth = theDesc->dwAlphaBitDepth;
        desc.dwBackBufferCount = theDesc->dwBackBufferCount;
        desc.dwMipMapCount = theDesc->dwMipMapCount;
        desc.dwReserved = theDesc->dwReserved;
        desc.lpSurface = theDesc->lpSurface;
        desc.ddpfPixelFormat = theDesc->ddpfPixelFormat;

        res = mDD->CreateSurface(&desc, theSurface, nullptr);
        if (!SUCCEEDED(res))
            return res;
    }

    // Make sure it's 32-bit or 16-bit
    DDSURFACEDESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_PIXELFORMAT;
    if (mPrimarySurface == nullptr)
        return DDERR_INVALIDPARAMS;

    mPrimarySurface->GetSurfaceDesc(&desc);

#ifdef OPTIMIZE_SOFTWARE_DRAWING
    // If things are stored blue low, green middle, red high, we can optimize a lot of our software rendering based on bit patterns.
    // This of course does not matter for native data which is already in the correct order (and can be optimized similarly).
    gOptimizeSoftwareDrawing = desc.ddpfPixelFormat.dwBBitMask < desc.ddpfPixelFormat.dwGBitMask && desc.ddpfPixelFormat.dwGBitMask < desc.ddpfPixelFormat.dwRBitMask;
#endif

    int aNumBits = desc.ddpfPixelFormat.dwRGBBitCount;
    if (aNumBits != 16 && aNumBits != 32)
    {
        (*theSurface)->Release();
        theSurface = nullptr;
        return DDERR_INVALIDPIXELFORMAT;
    }

    return res;
}

void DDInterface::ClearSurface(LPDIRECTDRAWSURFACE theSurface)
{
    // TODO: Implement
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
