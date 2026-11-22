#pragma once

#include "Common.h"
#include "CritSect.h"
#include "NativeDisplay.h"
#include "Rect.h"
#include "Ratio.h"

#include <ddraw.h>

#include "D3DTester.h"
#include "SharedImage.h"

namespace Sexy
{
    class IDXSurface;
    class RenderEffectDefinition;
    class D3DObject;
    class D3DInterface;
    class RenderDevice3D;
    class SexyAppBase;

    typedef std::set<DDImage*> DDImageSet;

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

        enum ERenderMode
        {
            RENDERMODE_Invalid = 0x0,
            RENDERMODE_Overdraw = 0x1,
            RENDERMODE_PseudoOverdraw = 0x2,
            RENDERMODE_BatchSize = 0x3,
            RENDERMODE_Wireframe = 0x4,
            RENDERMODE_WastedOverdraw = 0x5,
            RENDERMODE_TextureHash = 0x6,
            RENDERMODE_OverdrawExact = 0x7,
            RENDERMODE_COUNT = 0x8,
            RENDERMODE_CYCLE_END = 0x7,
        };

        enum ERenderModeFlags
        {
            RENDERMODEF_NoBatching = 0x1,
            RENDERMODEF_HalfTris = 0x2,
            RENDERMODEF_NoDynVB = 0x4,
            RENDERMODEF_PreventLag = 0x8,
            RENDERMODEF_NoTriRep = 0x10,
            RENDERMODEF_NoStretchRectFromTextures = 0x20,
            RENDERMODEF_HalfPresent = 0x40,
            RENDERMODEF_USEDBITS = 0x7,
        };

        D3DInterface* mD3DInterface;
        SexyAppBase* mApp;
        RenderDevice3D* mRenderDevice3D;
        D3DTester* mD3DTester;
        bool mIs3D;
        bool mWantD3D9;
        bool mIsD3D9;
        bool mIsD3D8;
        bool mIsD3D8Or9;
        CritSect mCritSect;
        bool mInRedraw;
        bool mInRedrawCursor;
        LPDIRECTDRAW mDD;
        LPDIRECTDRAW7 mDD7;
        LPDIRECTDRAWSURFACE mPrimarySurface;
        LPDIRECTDRAWSURFACE mSecondarySurface;
        LPDIRECTDRAWSURFACE mDrawSurface;
        IUnknown* mWindowScaleBuffers[4]; // TODO: Should this be LPDIRECTDRAWSURFACE?
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
        DDImageSet mDDImageSet;
        ulong mInitCount;
        int mCursorWidth;
        int mCursorHeight;
        int mNextCursorX;
        int mNextCursorY;
        int mCursorX;
        int mCursorY;
        Image* mCursorImage;
        bool mHasOldCursorArea;
        LPDIRECTDRAWSURFACE mOldCursorArea;
        LPDIRECTDRAWSURFACE mNewCursorArea;
        DDImage* mOldCursorAreaImage;
        DDImage* mNewCursorAreaImage;
        IDXSurface* mPrimaryDXSurface;
        std::string mErrorString;
        std::set<D3DObject*> mD3DObjectSet;
        std::vector<RenderEffectDefinition*> mRenderEffectDefs;
        ERenderMode mRenderMode;
        ulong mRenderModeFlags;

    public:
        void Cleanup();
        void Cleanup3DObjects();
	    bool GotDXError(HRESULT theResult, const char *theContext = "");

        HRESULT CreateSurface(DDSURFACEDESC2 *theDesc, LPDIRECTDRAWSURFACE *theSurface, void* unused);
        void ClearSurface(LPDIRECTDRAWSURFACE theSurface);
        bool Do3DTest(HWND theHWND);

    public:
        DDInterface(SexyAppBase* theApp);
        virtual ~DDInterface();

        void WindowResize(int width, int height);

	    static std::string ResultToString(int theResult);

        DDImage* GetScreenImage();
	    int Init(HWND theWindow, bool isWindowed);

	    bool SetCursorImage(Image* theImage);
    };

    extern bool gIs3D;
}
