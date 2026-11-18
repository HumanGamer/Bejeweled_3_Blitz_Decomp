#pragma once

#include "Common.h"
#include "CritSect.h"
#include "NativeDisplay.h"
#include "Rect.h"
#include "Ratio.h"

#include <ddraw.h>

#include "SharedImage.h"

namespace Sexy
{
    class D3DInterface;
    class RenderDevice3D;
    class SexyAppBase;

    class DDInterface : public NativeDisplay
    {
    public:
        enum EResult
        {
            RESULT_OK,
            RESULT_FAIL,
            RESULT_DD_CREATE_FAIL,
            RESULT_SURFACE_FAIL,
            RESULT_EXCLUSIVE_FAIL,
            RESULT_DISPCHANGE_FAIL,
            RESULT_INVALID_COLORDEPTH,
            RESULT_3D_FAIL,
            RESULT_3D_NOTREADY,
        };

        D3DInterface* mD3DInterface;
        SexyAppBase* mApp;
        RenderDevice3D* mRenderDevice3D;

        // TODO: Implement

        bool mIs3D;
        bool mWantD3D9;
        bool mIsD3D9;
        bool mIsD3D8;
        bool mIsD3D8Or9;
        CritSect mCritSect;
        bool mInRedraw;
        bool mInRedrawCursor;
        LPDIRECTDRAW mDD;

        // TODO: Implement

        int mWidth;
        int mHeight;
        Ratio mAspect;
        int mDesktopWidth;
        int mDesktopHeight;
        Ratio mDesktopAspect;
        bool mIsWidescreen;
        int mDisplayWidth;
        int mDisplayHeight;
        Ratio mDisplayAspect;
        float mFov;
        float mNearPlane;
        float mFarPlane;
        Rect mPresentationRect;
        int mFullscreenBits;
        DWORD mRefreshRate;
        DWORD mMillisecondsPerFrame;
        int mScanLineFailCount;
        int* mRedAddTable;
        int* mGreenAddTable;
        int* mBlueAddTable;
        uint mRedConvTable[256];
        uint mGreenConvTable[256];
        uint mBlueConvTable[256];
        bool mInitialized;
        HWND mHWnd;
        bool mIsWindowed;
        DDImage* mScreenImage;
        DDImage* mSecondarySurfaceImage;

        // TODO: Implement

        std::string mErrorString;

        // TODO: Implement

    public:
        void Cleanup();

    public:
        DDInterface(SexyAppBase* theApp);
        virtual ~DDInterface();

        void WindowResize(int width, int height);

	    static std::string ResultToString(int theResult);

        DDImage* GetScreenImage();
	    int Init(HWND theWindow, bool IsWindowed);

	    bool SetCursorImage(Image* theImage);
    };

    extern bool gIs3D;
}
