#include "SexyAppBase.h"

#include <ImageLib.h>
#include <PakInterface.h>
#include <time.h>
#include <shlobj.h>
#include <direct.h>

#include "BassMusicInterface.h"
#include "CfgMachine.h"
#include "DDInterface.h"
#include "Debug.h"
#include "DSoundManager.h"
#include "FModMusicInterface.h"
#include "MusicInterface.h"
#include "SEHCatcher.h"
#include "PopLoc.h"
#include "ResourceManager.h"
#include "SexyLocale.h"
#include "WidgetManager.h"

using namespace Sexy;

SexyAppBase* Sexy::gSexyAppBase = nullptr;

SEHCatcher Sexy::gSEHCatcher;

HMODULE gDDrawDLL = nullptr;
HMODULE gDSoundDLL = nullptr;
HMODULE gVersionDLL = nullptr;

typedef BOOL (WINAPI *GetLastInputInfoFunc)(LASTINPUTINFO *plii);
GetLastInputInfoFunc gGetLastInputInfoFunc = nullptr;
static bool gScreenSaverActive = false;

#ifndef SPI_GETSCREENSAVERRUNNING
#define SPI_GETSCREENSAVERRUNNING 114
#endif

//HotSpot: 11 4
//Size: 32 32
unsigned char gFingerCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc3,
	0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff,
	0xc0, 0x07, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xfc, 0x40, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff,
	0xfc, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01,
	0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0,
	0x03, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
	0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x18, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1b, 0x60, 0x00, 0x00, 0x1b, 0x68, 0x00,
	0x00, 0x1b, 0x6c, 0x00, 0x01, 0x9f, 0xec, 0x00, 0x01, 0xdf, 0xfc, 0x00, 0x00, 0xdf, 0xfc,
	0x00, 0x00, 0x5f, 0xfc, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x3f,
	0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00,
	0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

//HotSpot: 15 10
//Size: 32 32
unsigned char gDraggingCursorData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xf0, 0x07, 0xff, 0xff, 0xe0,
	0x01, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xff,
	0xe0, 0x00, 0xff, 0xfe, 0x60, 0x00, 0xff, 0xfc, 0x20, 0x00, 0xff, 0xfc, 0x00, 0x00, 0xff,
	0xfe, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00,
	0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0,
	0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff, 0xf8, 0x03, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x80, 0x00, 0x00, 0x01, 0xb0, 0x00, 0x00, 0x0d, 0xb0, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00,
	0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00, 0x00, 0x0d, 0xb6, 0x00,
	0x01, 0x8d, 0xb6, 0x00, 0x01, 0xcf, 0xfe, 0x00, 0x00, 0xef, 0xfe, 0x00, 0x00, 0xff, 0xfe,
	0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x1f,
	0xfc, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00,
	0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

SexyAppBase::SexyAppBase()
{
    gSexyAppBase = this;

    gVersionDLL = LoadLibraryA("version.dll");
    gDDrawDLL = LoadLibraryA("ddraw.dll");
    gDSoundDLL = LoadLibraryA("dsound.dll");
    gGetLastInputInfoFunc = (GetLastInputInfoFunc) GetProcAddress(GetModuleHandleA("user32.dll"), "GetLastInputInfo");

    ImageLib::InitJPEG2000();

    mMutex = nullptr;
    mNotifyGameMessage = 0;

#ifdef _DEBUG
    mOnlyAllowOneCopyToRun = false;
#else
    mOnlyAllowOneCopyToRun = true;
#endif

    // Extract product version
    char aPath[MAX_PATH];
    GetModuleFileNameA(nullptr, aPath, 256);
    mProductVersion = GetProductVersion(aPath);
    mChangeDirTo = GetFileDir(aPath);

    mNoDefer = false;
    mFullScreenPageFlip = true; // should we page flip in fullscreen?
    mTimeLoaded = GetTickCount();
    mSEHOccured = false;
    mProdName = "Product";
    mTitle = _S("SexyApp");
    mPendingUpdatesAcc = 0.0;
    mUpdateFTimeAcc = 0.0;
    mPreferredX = -1;
    mPreferredY = -1;
    mPreferredWidth = -1;
    mPreferredHeight = -1;
    mShutdown = false;
    mExitToTop = false;
    mWidth = 640;
    mHeight = 480;
    mFullscreenBits = 16;
    mIsWindowed = true;
    mIsPhysWindowed = true;
    mFullScreenWindow = false;
    mIsScreenSaver = false;
    mAllowMonitorPowersave = true;
    mWantsDialogCompatibility = false;
    mHWnd = nullptr;
    mDDInterface = nullptr;
    mMusicInterface = nullptr;
    mInvisHWnd = nullptr;
    mFrameTime = 10;
    mNonDrawCount = 0;
    mDrawCount = 0;
    mSleepCount = 0;
    mUpdateCount = 0;
    mUpdateAppState = 0;
    mUpdateAppDepth = 0;
    mHasPendingDraw = true;
    mIsDrawing = false;
    mLastDrawWasEmpty = false;
    mLastTimeCheck = 0;
    mUpdateMultiplier = 1.0;
    mMaxNonDrawCount = 10;
    mPaused = false;
    mReloadPct = 0.0f;
    mFastForwardToUpdateNum = 0;
    mFastForwardToMarker = false;
    mFastForwardStep = false;
    mSoundManager = nullptr;
    mCursorNum = 0;
    mMouseIn = false;
    mRunning = false;
    mActive = true;
    mProcessInTimer = false;
    mMinimized = false;
    mPhysMinimized = false;
    mIsDisabled = false;
    mLoaded = false;
    mReloadingResources = false;
    mYieldMainThread = false;
    mLoadingFailed = false;
    mLoadingThreadStarted = false;
    mAutoStartLoadingThread = true;
    mLoadingThreadCompleted = false;
    mCursorThreadRunning = false;
    mNumLoadingThreadTasks = 0;
    mCompletedLoadingThreadTasks = 0;
    mLastDrawTick = timeGetTime();
    mNextDrawTick = timeGetTime();
    mSysCursor = true;
    mMusicVolume = 0.85;
    mForceFullScreen = false;
    mSfxVolume = 0.85;
    mForceWindowed = false;
    mHasFocus = true;
    mDemoSfxVolume = 0.0;
    mCustomCursorsEnabled = false;
    mCustomCursorDirty = false;
    mOverrideCursor = nullptr;
    mIsOpeningURL = false;
    mInitialized = false;
    mLastShutdownWasGraceful = true;
    mReadFromRegistry = false;
    mCmdLineParsed = false;
    mSkipSignatureChecks = false;
    mCtrlDown = false;
    mAltDown = false;
    mAllowAltEnter = true;
    mStepMode = 0;
    mCleanupSharedImages = false;
    mStandardWordWrap = true;
    mbAllowExtendedChars = true;
    mEnableMaximizeButton = false;
    mWriteToSexyCache = true;
    mSexyCacheBuffers = false;
    mWriteFontCacheDir = true;
    mDemoMusicVolume = 0.0;
    mMuteCount = 0;
    mAutoMuteCount = 0;
    mDemoMute = false;
    mMuteOnLostFocus = true;
    mCurHandleNum = 0;
    mFPSTime = 0;
    mFPSStartTick = GetTickCount();
    mFPSFlipCount = 0;
    mFPSCount = 0;
    mFPSDirtyCount = 0;
    mShowFPS = false;
    mShowFPSMode = FPS_ShowFPS;
    mVFPSUpdateTimes = 0.0;
    mVFPSUpdateCount = 0;
    mVFPSDrawTimes = 0.0;
    mVFPSDrawCount = 0;
    mCurVFPS = 0.0;
    mDrawTime = 0;
    mScreenBltTime = 0;
    mDebugKeysEnabled = false;
    mOldWndProc = 0;
    mNoSoundNeeded = false;
    mWantFMod = false;
    mSyncRefreshRate = 100;
    mVSyncUpdates = false;
    mNoVSync = true;
    mVSyncBroken = false;
    mVSyncBrokenCount = 0;
    mVSyncBrokenTestStartTick = 0;
    mVSyncBrokenTestUpdates = 0;
    mWaitForVSync = false;
    mSoftVSyncWait = true;
    mAutoEnable3D = false;
    mTest3D = false;
    mNoD3D9 = false;
    mMinVidMemory3D = 6;
    mRecommendedVidMemory3D = 14;
    mRelaxUpdateBacklogCount = 0;
    mMaxUpdateBacklog = 200;
    mPauseWhenMoving = true;
    mWidescreenAware = false;
    mWidescreenTranslate = true;
    mEnableWindowAspect = false;
    mWindowAspect.Set(4, 3);
    mMinAspect.Set(4, 3);
    mMaxAspect.Set(16, 10);
    mIsWideWindow = false;
    mOrigScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    mOrigScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    mIsSizeCursor = false;
    mSharedImageEvent = CreateEventA(nullptr, false, false, nullptr);

    for (int i = 0; i < NUM_CURSORS; i++)
        mCursorImages[i] = nullptr;

    for (int i = 0; i < 256; i++)
        mAdd8BitMaxTable[i] = i;

    for (int i = 256; i < 512; i++)
        mAdd8BitMaxTable[i] = 255;

    // Set default strings.  Init could read in overrides from partner.xml
    SetString("UPDATE_CHECK_TITLE",     L"Update Check");
    SetString("UPDATE_CHECK_BODY",      L"Checking if there are any updates available for this product ...");

    SetString("UP_TO_DATE_TITLE",       L"Up to Date");
    SetString("UP_TO_DATE_BODY",        L"There are no updates available for this product at this time.");
    SetString("NEW_VERSION_TITLE",      L"New Version");
    SetString("NEW_VERSION_BODY",       L"There is an update available for this product.  Would you like to visit the web site to download it?");

    mDemoPrefix = "sexyapp";
    mDemoFileName = mDemoPrefix + ".dmo";
    mPlayingDemoBuffer = false;
    mManualShutdown = false;
    mRecordingDemoBuffer = false;
    mLastDemoMouseX = 0;
    mLastDemoMouseY = 0;
    mLastDemoUpdateCnt = 0;
    mDemoNeedsCommand = true;
    mDemoLoadingComplete = false;
    mDemoLength = 0;
    mDemoCmdNum = 0;
    mDemoCmdOrder = -1; // Means we haven't processed any demo commands yet
    mDemoCmdBitPos = 0;

    mWidgetManager = new WidgetManager(this);
    mResourceManager = new ResourceManager(this);

    mPrimaryThreadId = 0;

    if (GetSystemMetrics(SM_TABLETPC)) // check for tablet pc
    {
        mTabletPC = true;
        mFullScreenPageFlip = false; // so that tablet keyboard can show up
    }
    else
    {
        mTabletPC = false;
    }

    mSharedRTPool = nullptr;

    SEHCatcher::mApp = this;

    mAllowWindowResize = true;

    mCfgCompiler = ICfgCompiler::CompilerCreate();

    std::string errorBuffer;
    mCompatCfgMachine = mCfgCompiler->CompilerCreateMachineFromFile("compat.cfg", errorBuffer);

    if (mCompatCfgMachine != nullptr)
    {
        mCompatCfgMachine->MachineExecuteFunction(nullptr, nullptr);
    }
    else if (!errorBuffer.empty())
    {
        Popup(StrFormat("Configuration file parse error: %s", errorBuffer.c_str()));
    }

    mShowCompatInfoMode = SHOWCOMPATINFOMODE_OFF;
    mShowWidgetInspector = false;
    mWidgetInspectorCurWidget = nullptr;
    mWidgetInspectorScrollOffset = 0;
    mWidgetInspectorPickWidget = nullptr;
    mWidgetInspectorPickMode = false;
    mWidgetInspectorLeftAnchor = false;
    mWidgetInspectorClickPos.mX = -1;
    mWidgetInspectorClickPos.mY = -1;
}

SexyAppBase::~SexyAppBase()
{
    // TODO: Implement
}

typedef HRESULT (WINAPI *SHGetFolderPathFunc)(HWND, int, HANDLE, DWORD, LPTSTR);
static SHGetFolderPathFunc GetSHGetFolderPath(const char* theDLL, HMODULE* theMod)
{
    HMODULE aMod = LoadLibrary(theDLL);
    SHGetFolderPathFunc aFunc = nullptr;

    if (aMod != nullptr)
    {
        *((void**)&aFunc) = (void*)GetProcAddress(aMod, "SHGetFolderPathA");
        if (aFunc == nullptr)
        {
            FreeLibrary(aMod);
            aMod = nullptr;
        }
    }

    *theMod = aMod;
    return aFunc;
}

static BOOL CALLBACK ChangeDisplayWindowEnumProc(HWND hwnd, LPARAM lParam)
{
    typedef std::map<HWND,RECT> WindowMap;
    static WindowMap aMap;

    if (lParam == 0 && aMap.find(hwnd) == aMap.end()) // record
    {
        RECT aRect;
        if (!IsIconic(hwnd) && IsWindowVisible(hwnd))
        {
            if (GetWindowRect(hwnd, &aRect))
            {
                aMap[hwnd] = aRect;
            }
        }
    }
    else
    {
        WindowMap::iterator anItr = aMap.find(hwnd);
        if (anItr != aMap.end())
        {
            RECT &r = anItr->second;
            MoveWindow(hwnd, r.left, r.top, abs(r.right - r.left), abs(r.bottom - r.top), TRUE);
        }
    }
    return TRUE;
}

void SexyAppBase::Init()
{
    mPrimaryThreadId = GetCurrentThreadId();

    if (mShutdown)
        return;

    if (gDDrawDLL == nullptr || gDSoundDLL == nullptr)
    {
        MessageBox(nullptr,
                   GetString("APP_REQUIRES_DIRECTX", _S("This application requires DirectX to run.  You can get DirectX at http://www.microsoft.com/directx")).c_str(),
                   GetString("YOU_NEED_DIRECTX", _S("You need DirectX")).c_str(),
                   MB_OK | MB_ICONERROR);

        DoExit(0);
    }

    InitPropertiesHook();
    ReadFromRegistry();

    if (CheckForVista())
    {
        // TODO: GetSHGetFolderPath is deprecated and should be replaced.
        HMODULE aMod;
        SHGetFolderPathFunc aFunc = GetSHGetFolderPath("shell32.dll", &aMod);
        if (aFunc == nullptr || aMod == nullptr)
        {
            // This was originally re-declared as a separate variable which is clearly a bug.
            aFunc = GetSHGetFolderPath("shfolder.dll", &aMod);
        }

        if (aMod != nullptr)
        {
            char aPath[MAX_PATH];
            aFunc(nullptr, CSIDL_COMMON_APPDATA, nullptr, SHGFP_TYPE_CURRENT, aPath);

            std::string aDataPath = RemoveTrailingSlash(aPath) + "\\" + mFullCompanyName + "\\" + mProdName;
            SetAppDataFolder(aDataPath + "\\");
            //MkDir(aDataPath);
            //AllowAllAccess(aDataPath);

            if (mDemoFileName.length() < 2 || (mDemoFileName[1] != ':' && mDemoFileName[2] != '\\'))
            {
                mDemoFileName = GetAppDataFolder() + mDemoFileName;
            }

            FreeLibrary(aMod);
        }
    }

    if (!mCmdLineParsed)
        DoParseCmdLine();

    if (IsScreenSaver())
        mOnlyAllowOneCopyToRun = false;

    if (gHInstance == nullptr)
        gHInstance = GetModuleHandle(nullptr);

    // Change directory
    if (!ChangeDirHook(mChangeDirTo.c_str()))
        _chdir(mChangeDirTo.c_str());

    ImageLib::InitJPEG2000();

    gPakInterface->AddPakFile("main.pak");

    // Create a message we can use to talk to ourselves inter-process
    mNotifyGameMessage = RegisterWindowMessage((_S("Notify") + StringToSexyString(mProdName)).c_str());

    // Create a globally unique mutex
    mMutex = CreateMutex(nullptr, TRUE, (StringToSexyString(mProdName) + _S("Mutex")).c_str());
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        HandleGameAlreadyRunning();

    mRandSeed = GetTickCount();
    SRand(mRandSeed);

    // Set up demo recording stuff
    if (mPlayingDemoBuffer)
    {
        std::string anError;
        if (!ReadDemoBuffer(anError))
        {
            mPlayingDemoBuffer = false;
            Popup(anError);
            DoExit(0);
        }
    }

    srand(GetTickCount());

    if (CheckFor98Mill())
    {
        mIsWideWindow = false;

        WNDCLASSA wc;
        wc.style = CS_DBLCLKS;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = nullptr;
        wc.hCursor = nullptr;
        wc.hIcon = LoadIconA(gHInstance, "IDI_MAIN_ICON");
        wc.hInstance = gHInstance;
        wc.lpfnWndProc = WindowProc;
        wc.lpszClassName = "MainWindow";
        wc.lpszMenuName = nullptr;
        bool success = RegisterClassA(&wc) != 0;
        DBG_ASSERTE(success);
        if (mInvisHWnd == nullptr)
        {
            wc.style = 0;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hbrBackground = nullptr;
            wc.hCursor = nullptr;
            wc.hIcon = nullptr;
            wc.hInstance = gHInstance;
            wc.lpfnWndProc = WindowProc;
            wc.lpszClassName = "InvisWindow";
            wc.lpszMenuName = nullptr;
            success = RegisterClassA(&wc) != 0;
            DBG_ASSERTE(success);

            mInvisHWnd = CreateWindowExA(
                    0,
                    "InvisWindow",
                    SexyStringToStringFast(mTitle).c_str(),
                    0,
                    0,
                    0,
                    0,
                    0,
                    nullptr,
                    nullptr,
                    gHInstance,
                    0);
            SetWindowLong(mInvisHWnd, GWL_USERDATA, (LONG) this);
        }
    }
    else
    {
        mIsWideWindow = sizeof(SexyChar) == sizeof(wchar_t);

        WNDCLASS wc;
        wc.style = CS_DBLCLKS;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = nullptr;
        wc.hCursor = nullptr;
        wc.hIcon = LoadIconA(gHInstance, "IDI_MAIN_ICON");
        wc.hInstance = gHInstance;
        wc.lpfnWndProc = WindowProc;
        wc.lpszClassName = _S("MainWindow");
        wc.lpszMenuName = nullptr;
        bool success = RegisterClass(&wc) != 0;
        DBG_ASSERTE(success);

        if (mInvisHWnd == nullptr)
        {
            wc.style = 0;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hbrBackground = nullptr;
            wc.hCursor = nullptr;
            wc.hIcon = nullptr;
            wc.hInstance = gHInstance;
            wc.lpfnWndProc = WindowProc;
            wc.lpszClassName = _S("InvisWindow");
            wc.lpszMenuName = nullptr;
            success = RegisterClass(&wc) != 0;
            DBG_ASSERTE(success);

            mInvisHWnd = CreateWindowEx(
                    0,
                    _S("InvisWindow"),
                    mTitle.c_str(),
                    0,
                    0,
                    0,
                    0,
                    0,
                    nullptr,
                    nullptr,
                    gHInstance,
                    0);
            SetWindowLong(mInvisHWnd, GWL_USERDATA, (LONG) this);
        }
    }

    mHandCursor = CreateCursor(gHInstance, 11, 4, 32, 32, gFingerCursorData, gFingerCursorData + sizeof(gFingerCursorData) / 2);
    mDraggingCursor = CreateCursor(gHInstance, 15, 10, 32, 32, gDraggingCursorData, gDraggingCursorData+sizeof(gDraggingCursorData)/2);

    // Let app do something before showing window, or switching to fullscreen mode
    // NOTE: Moved call to PreDisplayHook above mIsWindowed and GetSystemsMetrics
    // checks because the checks below use values that could change in PreDisplayHook.
    // PreDisplayHook must call mWidgetManager->Resize if it changes mWidth or mHeight.
    PreDisplayHook();

    mWidgetManager->Resize(Rect(0, 0, mWidth, mHeight), Rect(0, 0, mWidth, mHeight));

    // Check to see if we CAN run windowed or not...
    /*if (mIsWindowed && !mFullScreenWindow)
    {
        // How can we be windowed if our screen isn't even big enough?
        if ((mWidth >= GetSystemMetrics(SM_CXFULLSCREEN)) ||
            (mHeight >= GetSystemMetrics(SM_CYFULLSCREEN)))
        {
            mIsWindowed = false;
            mForceFullscreen = true;
        }
    }*/

    if (mFullScreenWindow)
    {
        EnumWindows(ChangeDisplayWindowEnumProc, 0); // record window pos
        DEVMODE dm;
        EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm);

        // Switch resolutions
        if (dm.dmPelsWidth != mWidth || dm.dmPelsHeight != mHeight || (dm.dmBitsPerPel != 16 && dm.dmBitsPerPel != 32))
        {
            dm.dmPelsWidth = mWidth;
            dm.dmPelsHeight = mHeight;
            dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

            if (dm.dmBitsPerPel != 16 && dm.dmBitsPerPel != 32) // handle 24-bit/256 color case
            {
                dm.dmBitsPerPel = 16;
                dm.dmFields |= DM_BITSPERPEL;
            }

            if (ChangeDisplaySettings(&dm,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
            {
                mFullScreenWindow = false;
                mIsWindowed = false;
            }
        }
    }

    MakeWindow();

    if (mPlayingDemoBuffer)
    {
        // Get video data

        PrepareDemoCommand(true);
        mDemoNeedsCommand = true;

        DBG_ASSERTE(!mDemoIsShortCmd);
        DBG_ASSERTE(mDemoCmdNum == DEMO_VIDEO_DATA);

        mIsWindowed = mDemoBuffer.ReadBoolean();
        mSyncRefreshRate = mDemoBuffer.ReadByte();
    }

    if (mSoundManager == nullptr)
        mSoundManager = new DSoundManager(mNoSoundNeeded ? nullptr : mHWnd, mWantFMod);

    SetSfxVolume(mSfxVolume);

    mMusicInterface = CreateMusicInterface(mInvisHWnd);

    SetMusicVolume(mMusicVolume);

    if (IsScreenSaver())
    {
        SetCursor(CURSOR_NONE);
    }

    InitHook();

    Locale::SetLocale(GetString("LOCALE", _S("English_United States")));

    mInitialized = true;

    // TODO: Implement
}

MusicInterface* SexyAppBase::CreateMusicInterface(HWND theWindow)
{
    if (mNoSoundNeeded)
        return new MusicInterface;
    else if (mWantFMod)
        return new FModMusicInterface(mInvisHWnd);
    else
        return new BassMusicInterface(mInvisHWnd);
}

void SexyAppBase::InitHook() {}

void SexyAppBase::ReadFromRegistry()
{
    // TODO: Implement
}

void SexyAppBase::PreDisplayHook() {}

void SexyAppBase::HandleGameAlreadyRunning()
{
    if (mOnlyAllowOneCopyToRun)
    {
        // Notify the other window and then shut ourselves down
        if (mNotifyGameMessage != 0)
            PostMessage(HWND_BROADCAST, mNotifyGameMessage, 0, 0);

        DoExit(0);
    }
}

void SexyAppBase::Start()
{
    // TODO: Implement
}

void SexyAppBase::Shutdown()
{
    // TODO: Implement
}

void SexyAppBase::DoParseCmdLine()
{
    char* aCmdLine = GetCommandLineA();
    char* aCmdLinePtr = aCmdLine;
    if (aCmdLinePtr[0] == '"')
    {
        aCmdLinePtr = strchr(aCmdLinePtr + 1, '"');
        if (aCmdLinePtr != nullptr)
            aCmdLinePtr++;
    }

    if (aCmdLinePtr != nullptr)
    {
        aCmdLinePtr = strchr(aCmdLinePtr, ' ');
        if (aCmdLinePtr != nullptr)
            ParseCmdLine(aCmdLinePtr + 1);
    }

    mCmdLineParsed = true;
}

void SexyAppBase::ParseCmdLine(const std::string &theCmdLine)
{
    // TODO: Implement
}

bool SexyAppBase::ChangeDirHook(const char *theIntendedPath)
{
    return false;
}

void SexyAppBase::RestoreScreenResolution()
{
    if (mFullScreenWindow)
    {
        EnumWindows(ChangeDisplayWindowEnumProc, 0); // get any windows that appeared while we were running
        ChangeDisplaySettings(nullptr, 0);
        EnumWindows(ChangeDisplayWindowEnumProc, 1); // restore window pos
        mFullScreenWindow = false;
    }
}

void SexyAppBase::DoExit(int theCode)
{
    RestoreScreenResolution();
    exit(theCode);
}

void SexyAppBase::SetCursor(int theCursorNum)
{
    mCursorNum = theCursorNum;
    EnforceCursor();
}

void SexyAppBase::ClearKeysDown()
{
    if (mWidgetManager != nullptr) // fix stuck alt-key problem
    {
        for (int aKeyNum = 0; aKeyNum < 0xFF; aKeyNum++)
            mWidgetManager->mKeyDown[aKeyNum] = false;
    }

    mCtrlDown = false;
    mAltDown = false;
}

void SexyAppBase::BeginPopup()
{
    if (!mIsPhysWindowed)
    {
        if (mDDInterface && mDDInterface->mDD)
        {
            mDDInterface->mDD->FlipToGDISurface();
            mNoDefer = true;
        }
    }
}

void SexyAppBase::EndPopup()
{
    if (!mIsPhysWindowed)
        mNoDefer = false;

    ClearUpdateBacklog();
    ClearKeysDown();

    if (mWidgetManager->mDownButtons)
    {
        mWidgetManager->DoMouseUps();
        ReleaseCapture();
    }
}

int SexyAppBase::MsgBox(const std::string& theText, const std::string& theTitle, int theFlags)
{
    if (IsScreenSaver())
    {
        LogScreenSaverError(theText);
        return IDOK;
    }

    BeginPopup();
    int aResult = MessageBoxA(mHWnd, theText.c_str(), theTitle.c_str(), theFlags);
    EndPopup();

    return aResult;
}

int SexyAppBase::MsgBox(const std::wstring& theText, const std::wstring& theTitle, int theFlags)
{
    if (IsScreenSaver())
    {
        LogScreenSaverError(WStringToString(theText));
        return IDOK;
    }

    BeginPopup();
    int aResult = MessageBoxW(mHWnd, theText.c_str(), theTitle.c_str(), theFlags);
    EndPopup();

    return aResult;
}

void SexyAppBase::Popup(const std::string& theString)
{
    if (IsScreenSaver())
    {
        LogScreenSaverError(theString);
        return;
    }

    BeginPopup();
    if (!mShutdown)
        ::MessageBoxA(mHWnd, theString.c_str(), SexyStringToString(GetString("FATAL_ERROR", _S("FATAL ERROR"))).c_str(), MB_ICONERROR);
    EndPopup();
}

void SexyAppBase::Popup(const std::wstring& theString)
{
    if (IsScreenSaver())
    {
        LogScreenSaverError(WStringToString(theString));
        return;
    }

    BeginPopup();
    if (!mShutdown)
        ::MessageBoxW(mHWnd, theString.c_str(), SexyStringToWString(GetString("FATAL_ERROR", _S("FATAL ERROR"))).c_str(), MB_ICONERROR);
    EndPopup();
}

void SexyAppBase::LogScreenSaverError(const std::string &theError)
{
    static bool firstTime = true;
    char aBuf[512];

    const char *aFlag = firstTime?"w":"a+";
    firstTime = false;

    FILE *aFile = fopen("ScrError.txt",aFlag);
    if (aFile != NULL)
    {
        fprintf(aFile,"%s %s %u\n",theError.c_str(),_strtime(aBuf),GetTickCount());
        fclose(aFile);
    }
}

std::string SexyAppBase::GetProductVersion(const std::string& thePath)
{
    // TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

    // Dynamically Load Version.dll
    typedef DWORD (APIENTRY *GetFileVersionInfoSizeFunc)(LPSTR lptstrFilename, LPDWORD lpdwHandle);
    typedef BOOL (APIENTRY *GetFileVersionInfoFunc)(LPSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
    typedef BOOL (APIENTRY *VerQueryValueFunc)(const LPVOID pBlock, LPSTR lpSubBlock, LPVOID * lplpBuffer, PUINT puLen);

    static GetFileVersionInfoSizeFunc aGetFileVersionInfoSizeFunc = nullptr;
    static GetFileVersionInfoFunc aGetFileVersionInfoFunc = nullptr;
    static VerQueryValueFunc aVerQueryValueFunc = nullptr;

    if (aGetFileVersionInfoSizeFunc == nullptr)
    {
        aGetFileVersionInfoSizeFunc = (GetFileVersionInfoSizeFunc)GetProcAddress(gVersionDLL,"GetFileVersionInfoSizeA");
        aGetFileVersionInfoFunc = (GetFileVersionInfoFunc)GetProcAddress(gVersionDLL,"GetFileVersionInfoA");
        aVerQueryValueFunc = (VerQueryValueFunc)GetProcAddress(gVersionDLL,"VerQueryValueA");
    }

    // Get Product Version
    std::string aProductVersion;

    uint aSize = aGetFileVersionInfoSizeFunc((char*) thePath.c_str(), 0);
    if (aSize > 0)
    {
        uchar* aVersionBuffer = new uchar[aSize];
        aGetFileVersionInfoFunc((char*) thePath.c_str(), 0, aSize, aVersionBuffer);
        char* aBuffer;
        if (aVerQueryValueFunc(aVersionBuffer,
                  LPSTR("\\StringFileInfo\\040904B0\\ProductVersion"),
                  (void**) &aBuffer,
                  &aSize))
        {
            aProductVersion = aBuffer;
        }
        else if (aVerQueryValueFunc(aVersionBuffer,
                  LPSTR("\\StringFileInfo\\040904E4\\ProductVersion"),
                  (void**) &aBuffer,
                  &aSize))
        {
            aProductVersion = aBuffer;
        }

        delete aVersionBuffer;
    }

    return aProductVersion;
}

void SexyAppBase::SEHOccured()
{
    SetMusicVolume(0);
    ::ShowWindow(mHWnd, SW_HIDE);
    mSEHOccured = true;
    EnforceCursor();
}

void SexyAppBase::MakeWindow()
{
    // TODO: Implement
}

void SexyAppBase::EnforceCursor()
{
    // TODO: Implement
}

void SexyAppBase::SetMusicVolume(double theVolume)
{
    // TODO: Implement
}

void SexyAppBase::SetSfxVolume(double theVolume)
{
    // TODO: Implement
}

void SexyAppBase::InitPropertiesHook() {}

void SexyAppBase::SetString(const std::string& theId, const std::wstring& theValue)
{
    std::pair<StringWStringMap::iterator, bool> aPair = mStringProperties.insert(StringWStringMap::value_type(theId, theValue));
    if (!aPair.second) // Found it, change value
        aPair.first->second = theValue;

    // numeric IDs also go into the localization table
    int anId;
    if (StringToInt(theId, &anId))
        mPopLoc->SetString(anId, ToSexyString(theValue), false);
}

SexyString SexyAppBase::GetString(const std::string& theId)
{
    StringWStringMap::iterator anItr = mStringProperties.find(theId);
    DBG_ASSERTE(anItr != mStringProperties.end());

    if (anItr != mStringProperties.end())
        return WStringToSexyString(anItr->second);
    else
        return _S("");
}

SexyString SexyAppBase::GetString(const std::string& theId, const SexyString& theDefault)
{
    StringWStringMap::iterator anItr = mStringProperties.find(theId);

    if (anItr != mStringProperties.end())
        return WStringToSexyString(anItr->second);
    else
        return theDefault;
}

bool SexyAppBase::PrepareDemoCommand(bool required)
{
    // TODO: Implement
    return false;
}

bool SexyAppBase::ReadDemoBuffer(std::string &theError)
{
    // TODO: Implement
    return false;
}

void SexyAppBase::ClearUpdateBacklog(bool relaxForASecond)
{
    mLastTimeCheck = timeGetTime();
    mUpdateFTimeAcc = 0.0;

    if (relaxForASecond)
        mRelaxUpdateBacklogCount = 1000;
}

bool SexyAppBase::IsScreenSaver()
{
    return mIsScreenSaver;
}

static bool ScreenSaverWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &theResult)
{
    // TODO: Implement
    return false;
}

LRESULT CALLBACK SexyAppBase::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (gSexyAppBase != nullptr && gSexyAppBase->IsScreenSaver())
    {
        LRESULT aResult;
        if (ScreenSaverWindowProc(hWnd, uMsg, wParam, lParam, aResult))
            return aResult;
    }

    SexyAppBase* aSexyApp = (SexyAppBase*) GetWindowLong(hWnd, GWL_USERDATA);

    // TODO: Implement

    if (gSexyAppBase->mIsWideWindow)
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    else
        return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}
