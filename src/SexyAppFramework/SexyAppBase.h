#pragma once

#include "Buffer.h"
#include "ButtonListener.h"
#include "Common.h"
#include "CritSect.h"
#include "DialogListener.h"
#include "Ratio.h"
#include "Rect.h"
#include "SharedImage.h"
#include "SharedRenderTarget.h"

namespace ImageLib
{
    class Image;
};

namespace Sexy
{
    class ICfgMachine;
    class ICfgCompiler;
    class PopLoc;
    class PopAnim;
    class PIEffect;
    class ImageFont;
    class WidgetContainer;
    class WidgetManager;
    class DDInterface;
    class Image;
    class DDImage;
    class Widget;
    class SoundManager;
    class MusicInterface;
    class MemoryImage;
    class HTTPTransfer;
    class Dialog;

    class ResourceManager;

    class WidgetSafeDeleteInfo
    {
    public:
        int						mUpdateAppDepth;
        Widget*					mWidget;
    };

    typedef std::list<WidgetSafeDeleteInfo> WidgetSafeDeleteList;
    typedef std::set<MemoryImage*> MemoryImageSet;
    typedef std::set<ImageFont*> ImageFontSet;
    typedef std::set<PIEffect*> PIEffectSet;
    typedef std::set<PopAnim*> PopAnimSet;
    typedef std::map<int, Dialog*> DialogMap;
    typedef std::list<Dialog*> DialogList;
    typedef std::list<MSG> WindowsMessageList;
    typedef std::vector<std::string> StringVector;

    typedef std::map<std::string, SexyString> StringSexyStringMap;
    typedef std::map<std::string, std::string> StringStringMap;
    typedef std::map<std::string, std::wstring> StringWStringMap;
    typedef std::map<std::string, bool> StringBoolMap;
    typedef std::map<std::string, int> StringIntMap;
    typedef std::map<std::string, double> StringDoubleMap;
    typedef std::map<std::string, StringVector> StringStringVectorMap;

    enum
    {
        CURSOR_POINTER,
        CURSOR_HAND,
        CURSOR_DRAGGING,
        CURSOR_TEXT,
        CURSOR_CIRCLE_SLASH,
        CURSOR_SIZEALL,
        CURSOR_SIZENESW,
        CURSOR_SIZENS,
        CURSOR_SIZENWSE,
        CURSOR_SIZEWE,
        CURSOR_WAIT,
        CURSOR_NONE,
        CURSOR_CUSTOM,
        NUM_CURSORS,
    };

    enum
    {
        DEMO_MOUSE_POSITION,
        DEMO_ACTIVATE_APP,
        DEMO_SIZE,
        DEMO_KEY_DOWN,
        DEMO_KEY_UP,
        DEMO_KEY_CHAR,
        DEMO_CLOSE,
        DEMO_MOUSE_ENTER,
        DEMO_MOUSE_EXIT,
        DEMO_LOADING_COMPLETE,
        DEMO_REGISTRY_GETSUBKEYS,
        DEMO_REGISTRY_READ,
        DEMO_REGISTRY_WRITE,
        DEMO_REGISTRY_ERASE,
        DEMO_FILE_EXISTS,
        DEMO_FILE_READ,
        DEMO_FILE_WRITE,
        DEMO_HTTP_RESULT,
        DEMO_SYNC,
        DEMO_ASSERT_STRING_EQUAL,
        DEMO_ASSERT_INT_EQUAL,
        DEMO_MOUSE_WHEEL,
        DEMO_HANDLE_COMPLETE,
        DEMO_VIDEO_DATA,
        DEMO_IDLE = 31
    };

    enum {
        FPS_ShowFPS,
        FPS_ShowCoords,
        Num_FPS_Types
    };

    enum
    {
        UPDATESTATE_MESSAGES,
        UPDATESTATE_PROCESS_1,
        UPDATESTATE_PROCESS_2,
        UPDATESTATE_PROCESS_DONE
    };

    typedef std::map<HANDLE, int> HandleToIntMap;

    class SexyAppBase : public ButtonListener, DialogListener
    {
    public:
        ulong mRandSeed;

        std::string mCompanyName;
        std::string mFullCompanyName;
        std::string mProdName;
        SexyString mTitle;
        std::string mRegKey;
        std::string mChangeDirTo;

        int mRelaxUpdateBacklogCount; // app doesn't try to catch up for this many frames
        int mMaxUpdateBacklog;
        bool mPauseWhenMoving;
        int mPreferredX;
        int mPreferredY;
        int mPreferredWidth;
        int mPreferredHeight;
        int mWidth;
        int mHeight;
        int mFullscreenBits;
        double mMusicVolume;
        double mSfxVolume;
        double mDemoMusicVolume;
        double mDemoSfxVolume;
        bool mNoSoundNeeded;
        bool mWantFMod;
        bool mCmdLineParsed;
        bool mSkipSignatureChecks;
        bool mStandardWordWrap;
        bool mbAllowExtendedChars;

        HANDLE mMutex;
        bool mOnlyAllowOneCopyToRun;
        UINT mNotifyGameMessage;
        CritSect mCritSect;
        CritSect mGetImageCritSect;
        bool mBetaValidate;
        uchar mAdd8BitMaxTable[512];
        WidgetManager* mWidgetManager;
        DialogMap mDialogMap;
        DialogList mDialogList;
        DWORD mPrimaryThreadId;
        bool mSEHOccured;
        bool mShutdown;
        bool mExitToTop;
        bool mIsWindowed;
        bool mIsPhysWindowed;
        bool mFullScreenWindow; // uses ChangeDisplaySettings to run fullscreen with mIsWindowed true
        bool mForceFullScreen;
        bool mForceWindowed;
        bool mInitialized;
        bool mProcessInTimer;
        DWORD mTimeLoaded;
        HWND mHWnd;
        HWND mInvisHWnd;
        bool mIsScreenSaver;
        bool mAllowMonitorPowersave;
        bool mWantsDialogCompatibility;
        WindowsMessageList mDeferredMessages;
        bool mNoDefer;
        bool mFullScreenPageFlip;
        bool mTabletPC;
        DDInterface* mDDInterface;
        MusicInterface* mMusicInterface;
        bool mReadFromRegistry;
        std::string mRegisterLink;
        std::string mProductVersion;
        Image* mCursorImages[NUM_CURSORS];
        HCURSOR mOverrideCursor;
        bool mIsOpeningURL;
        bool mShutdownOnURLOpen;
        std::string mOpeningURL;
        DWORD mOpeningURLTime;
        DWORD mLastTimerTime;
        DWORD mLastBigDelayTime;
        double mUnmutedMusicVolume;
        double mUnmutedSfxVolume;
        int mMuteCount;
        int mAutoMuteCount;
        bool mDemoMute;
        bool mMuteOnLostFocus;
        MemoryImageSet mMemoryImageSet;
        CritSect mImageSetCritSect;
        ImageFontSet mImageFontSet;
        PIEffectSet mPIEffectSet;
        PopAnimSet mPopAnimSet;
        SharedImageMap mSharedImageMap;
        HANDLE mSharedImageEvent;
        bool mCleanupSharedImages;

        int mNonDrawCount;
        int mFrameTime;

        bool mIsDrawing;
        bool mLastDrawWasEmpty;
        bool mHasPendingDraw;
        double mPendingUpdatesAcc;
        double mUpdateFTimeAcc;
        DWORD mLastTimeCheck;
        DWORD mLastTime;
        DWORD mLastUserInputTick;

        int mSleepCount;
        int mDrawCount;
        int mUpdateCount;
        int mUpdateAppState;
        int mUpdateAppDepth;
        int mMaxNonDrawCount;
        double mUpdateMultiplier;
        bool mPaused;
        int mFastForwardToUpdateNum;
        bool mFastForwardToMarker;
        bool mFastForwardStep;
        DWORD mLastDrawTick;
        DWORD mNextDrawTick;
        int mStepMode; // 0 = off, 1 = step, 2 = waiting for step

        int mCursorNum;
        SoundManager* mSoundManager;
        HCURSOR mHandCursor;
        HCURSOR mDraggingCursor;
        WidgetSafeDeleteList mSafeDeleteList;
        bool mMouseIn;
        bool mRunning;
        bool mActive;
        bool mMinimized;
        bool mPhysMinimized;
        bool mIsDisabled;
        bool mHasFocus;
        int mDrawTime;
        ulong mFPSStartTick;
        int mFPSFlipCount;
        int mFPSDirtyCount;
        int mFPSTime;
        int mFPSCount;
        bool mShowFPS;
        int mShowFPSMode;
        double mVFPSUpdateTimes;
        int mVFPSUpdateCount;
        double mVFPSDrawTimes;
        int mVFPSDrawCount;
        float mCurVFPS;
        int mScreenBltTime;
        bool mAutoStartLoadingThread;
        bool mLoadingThreadStarted;
        bool mLoadingThreadCompleted;
        bool mLoaded;
        bool mReloadingResources;
        float mReloadPct;
        std::string mReloadText;
        std::string mReloadSubText;
        bool mYieldMainThread;
        bool mLoadingFailed;
        bool mCursorThreadRunning;
        bool mSysCursor;
        bool mCustomCursorsEnabled;
        bool mCustomCursorDirty;
        bool mLastShutdownWasGraceful;
        bool mIsWideWindow;
        bool mWriteToSexyCache;
        bool mSexyCacheBuffers;
        bool mWriteFontCacheDir;

        int mNumLoadingThreadTasks;
        int mCompletedLoadingThreadTasks;

        // For recording/playback of program control
        bool mRecordingDemoBuffer;
        bool mPlayingDemoBuffer;
        bool mManualShutdown;
        std::string mDemoPrefix;
        std::string mDemoFileName;
        Buffer mDemoBuffer;
        int mDemoLength;
        int mLastDemoMouseX;
        int mLastDemoMouseY;
        int mLastDemoUpdateCnt;
        bool mDemoNeedsCommand;
        bool mDemoIsShortCmd;
        int mDemoCmdNum;
        int mDemoCmdOrder;
        int mDemoCmdBitPos;
        bool mDemoLoadingComplete;
        HandleToIntMap mHandleToIntMap; // For waiting on handles
        int mCurHandleNum;

        typedef std::pair<std::string, int> DemoMarker;
        typedef std::list<DemoMarker> DemoMarkerList;
        DemoMarkerList mDemoMarkerList;

        bool mDebugKeysEnabled;
        bool mEnableMaximizeButton;
        bool mCtrlDown;
        bool mAltDown;
        bool mAllowAltEnter;

        int mSyncRefreshRate;
        bool mVSyncUpdates;
        bool mNoVSync;
        bool mVSyncBroken;
        int mVSyncBrokenCount;
        DWORD mVSyncBrokenTestStartTick;
        DWORD mVSyncBrokenTestUpdates;
        bool mWaitForVSync;
        bool mSoftVSyncWait;
        bool mAutoEnable3D;
        bool mTest3D;
        bool mNoD3D9;
        DWORD mMinVidMemory3D;
        DWORD mRecommendedVidMemory3D;

        bool mWidescreenAware;
        bool mWidescreenTranslate;
        Rect mScreenBounds;
        bool mEnableWindowAspect;
        Ratio mWindowAspect;
        Ratio mMinAspect;
        Ratio mMaxAspect;
        bool mAllowWindowResize;
        int mOrigScreenWidth;
        int mOrigScreenHeight;
        bool mIsSizeCursor;

        StringWStringMap mStringProperties;
        StringBoolMap mBoolProperties;
        StringIntMap mIntProperties;
        StringDoubleMap mDoubleProperties;
        StringStringVectorMap mStringVectorProperties;
        ResourceManager* mResourceManager;
        PopLoc* mPopLoc;
        SharedRenderTarget::Pool *mSharedRTPool;
        ICfgCompiler* mCfgCompiler;
        ICfgMachine* mCompatCfgMachine;

        enum EShowCompatInfoMode
        {
            SHOWCOMPATINFOMODE_OFF,
            SHOWCOMPATINFOMODE_BOTTOM,
            SHOWCOMPATINFOMODE_TOP,
            SHOWCOMPATINFOMODE_COUNT,
        } mShowCompatInfoMode;

        bool mShowWidgetInspector;
        bool mWidgetInspectorPickMode;
        bool mWidgetInspectorLeftAnchor;
        WidgetContainer* mWidgetInspectorPickWidget;
        WidgetContainer* mWidgetInspectorCurWidget;
        int mWidgetInspectorScrollOffset;
        Point mWidgetInspectorClickPos;

        LONG mOldWndProc;

    protected:
	    void RehupFocus();
        void ClearKeysDown();

        virtual void MakeWindow();
        virtual void EnforceCursor();
        virtual void ReInitImages();
        
        void RestoreScreenResolution();
        void DoExit(int theCode);

    public:
        SexyAppBase();
        virtual ~SexyAppBase();

        // Common overrides:
	    virtual MusicInterface*	CreateMusicInterface(HWND theHWnd);
        virtual void InitHook();
        virtual void ReadFromRegistry();
        virtual void PreDisplayHook();

        virtual void HandleGameAlreadyRunning();

        virtual void Start();
        virtual void Init();
        virtual void Shutdown();

        virtual void DoParseCmdLine();
	    virtual void ParseCmdLine(const std::string& theCmdLine);

        virtual bool ChangeDirHook(const char *theIntendedPath);

        void SetCursor(int theCursorNum);


        virtual void BeginPopup();
        virtual void EndPopup();
        virtual int MsgBox(const std::string &theText, const std::string &theTitle = "Message", int theFlags = MB_OK);
        virtual int MsgBox(const std::wstring &theText, const std::wstring &theTitle = L"Message", int theFlags = MB_OK);
        virtual void Popup(const std::string& theString);
        virtual void Popup(const std::wstring& theString);
	    virtual void LogScreenSaverError(const std::string &theError);

        virtual std::string GetProductVersion(const std::string& thePath);

        virtual void SEHOccured();

        virtual void SetMusicVolume(double theVolume);
        virtual void SetSfxVolume(double theVolume);
        virtual void Mute(bool autoMute = false);
        virtual void Unmute(bool autoMute = false);

        virtual void InitPropertiesHook();

        void SetString(const std::string& theId, const std::wstring& theValue);

        SexyString GetString(const std::string& theId);
        SexyString GetString(const std::string& theId, const SexyString& theDefault);

        bool RegistryReadBoolean(const std::string& theValueName, bool* theValue);

        bool PrepareDemoCommand(bool required);
	    bool ReadDemoBuffer(std::string &theError);//UNICODE

        bool Is3DAccelerated();
	    void Set3DAccelerated(bool is3D, bool reinit = true);

        virtual void SwitchScreenMode();
        virtual void SwitchScreenMode(bool wantWindowed);
        virtual void SwitchScreenMode(bool wantWindowed, bool is3d, bool force = false);

        int InitDDInterface();
        void ClearUpdateBacklog(bool relaxForASecond = false);
        bool IsScreenSaver();

	    static LRESULT CALLBACK	WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };

    extern SexyAppBase* gSexyAppBase;

}
