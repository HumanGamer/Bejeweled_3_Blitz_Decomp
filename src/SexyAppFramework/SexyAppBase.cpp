#include "SexyAppBase.h"

#include <ImageLib.h>
#include <PakInterface.h>
#include <time.h>
#include <shlobj.h>
#include <direct.h>
#include <process.h>

#include "BassMusicInterface.h"
#include "CfgMachine.h"
#include "DDImage.h"
#include "DDInterface.h"
#include "Debug.h"
#include "DSoundManager.h"
#include "FModMusicInterface.h"
#include "MusicInterface.h"
#include "PerfTimer.h"
#include "SEHCatcher.h"
#include "PopLoc.h"
#include "PropertiesParser.h"
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

void SexyAppBase::PreTerminate() {}

void SexyAppBase::WriteToRegistry()
{
    // TODO: Implement
}

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

void SexyAppBase::HandleNotifyGameMessage(int theType, int theParam)
{
    if (theType==0) // bring to front message
    {
        WINDOWPLACEMENT aWindowPlacement;
        aWindowPlacement.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(mHWnd, &aWindowPlacement);

        if (aWindowPlacement.showCmd == SW_SHOWMINIMIZED)
            ShowWindow(mHWnd, SW_RESTORE);

        ::SetForegroundWindow(mHWnd);
    }
}

bool SexyAppBase::IsAltKeyUsed(WPARAM wParam)
{

    int aChar = tolower(wParam);
    switch (aChar)
    {
        case '\r': // alt-enter
        case 'a':
        case 'r':
        case 's':
            return true;
        default:
            return false;
    }
}

bool SexyAppBase::DebugKeyDown(int theKey)
{
    // TODO: Implement
    return false;
}

bool SexyAppBase::DebugKeyDownAsync(int theKey, bool ctrlDown, bool altDown)
{
    return false;
}

void SexyAppBase::CloseRequestAsync() {}

void SexyAppBase::DoMainLoop()
{
    while (!mShutdown)
    {
        if (mExitToTop)
            mExitToTop = false;
        UpdateApp();
    }
}

bool SexyAppBase::UpdateAppStep(bool* updated)
{
    if (updated != nullptr)
        *updated = false;

    if (mExitToTop)
        return false;

    if (mUpdateAppState == UPDATESTATE_PROCESS_DONE)
        mUpdateAppState = UPDATESTATE_MESSAGES;

    mUpdateAppDepth++;

    // We update in two stages to avoid doing a Process if our loop termination
    //  condition has already been met by processing windows messages
    if (mUpdateAppState == UPDATESTATE_MESSAGES)
    {
        MSG msg;
        while ((PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) && (!mShutdown))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ProcessDemo();
        if (!ProcessDeferredMessages(true))
        {
            mUpdateAppState = UPDATESTATE_PROCESS_1;
        }
    }
    else
    {
        // Process changes state by itself
        if (mStepMode)
        {
            if (mStepMode==2)
            {
                Sleep(mFrameTime);
                mUpdateAppState = UPDATESTATE_PROCESS_DONE; // skip actual update until next step
            }
            else
            {
                mStepMode = 2;
                DoUpdateFrames();
                DoUpdateFramesF(1.0f);
                DrawDirtyStuff();
            }
        }
        else
        {
            int anOldUpdateCnt = mUpdateCount;
            Process();
            if (updated != NULL)
                *updated = mUpdateCount != anOldUpdateCnt;
        }
    }

    mUpdateAppDepth--;

    return true;
}

bool SexyAppBase::UpdateApp()
{
    bool updated;
    for (;;)
    {
        if (!UpdateAppStep(&updated))
            return false;
        if (updated)
            return true;
    }
}

void SexyAppBase::Start()
{
    if (mShutdown)
        return;

    StartCursorThread();

    if (mAutoStartLoadingThread)
        StartLoadingThread();

    ShowWindow(mHWnd, SW_SHOW);
    SetFocus(mHWnd);

    timeBeginPeriod(1);

    int aCount = 0;
    int aSleepCount = 0;

    DWORD aStartTime = timeGetTime();

    mRunning = true;
    mLastTime = aStartTime;
    mLastUserInputTick = aStartTime;
    mLastTimerTime = aStartTime;

    DoMainLoop();
    ProcessSafeDeleteList();

    mRunning = false;

    WaitForLoadingThread();

    char aString[256];
    sprintf(aString, "Seconds       = %g\r\n", (timeGetTime() - aStartTime) / 1000.0);
    OutputDebugStringA(aString);
    //sprintf(aString, "Count         = %d\r\n", aCount);
    //OutputDebugString(aString);
    sprintf(aString, "Sleep Count   = %d\r\n", mSleepCount);
    OutputDebugStringA(aString);
    sprintf(aString, "Update Count  = %d\r\n", mUpdateCount);
    OutputDebugStringA(aString);
    sprintf(aString, "Draw Count    = %d\r\n", mDrawCount);
    OutputDebugStringA(aString);
    sprintf(aString, "Draw Time     = %d\r\n", mDrawTime);
    OutputDebugStringA(aString);
    sprintf(aString, "Screen Blt    = %d\r\n", mScreenBltTime);
    OutputDebugStringA(aString);
    if (mDrawTime + mScreenBltTime > 0)
    {
        sprintf(aString, "Avg FPS       = %d\r\n", (mDrawCount * 1000) / (mDrawTime + mScreenBltTime));
        OutputDebugStringA(aString);
    }

    timeEndPeriod(1);

    PreTerminate();

    WriteToRegistry();
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

void SexyAppBase::StartCursorThread()
{
    // TODO: Implement
}

void SexyAppBase::WaitForLoadingThread()
{
    while ((mLoadingThreadStarted) && (!mLoadingThreadCompleted))
        Sleep(20);
}

void SexyAppBase::ProcessSafeDeleteList()
{
    // TODO: Implement
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

void SexyAppBase::ProcessDemo()
{
    // TODO: Implement
}

void SexyAppBase::SetCursor(int theCursorNum)
{
    mCursorNum = theCursorNum;
    EnforceCursor();
}

void SexyAppBase::CleanSharedImages()
{
    // TODO: Implement
}

void SexyAppBase::RehupFocus()
{
    // TODO: Implement
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

// Why did I defer messages?  Oh, incase a dialog comes up such as a crash
//  it won't keep crashing and stuff
bool SexyAppBase::ProcessDeferredMessages(bool singleMessage)
{
    // TODO: Implement
    return false;
}

void SexyAppBase::UpdateFTimeAcc()
{
    DWORD aCurTime = timeGetTime();

    if (mLastTimeCheck != 0)
    {
        int aDeltaTime = aCurTime - mLastTimeCheck;

        mUpdateFTimeAcc = min(mUpdateFTimeAcc + aDeltaTime, mMaxUpdateBacklog);

        if (mRelaxUpdateBacklogCount > 0)
            mRelaxUpdateBacklogCount = max(mRelaxUpdateBacklogCount - aDeltaTime, 0);
    }

    mLastTimeCheck = aCurTime;
}

bool SexyAppBase::Process(bool allowSleep)
{
    if (mLoadingFailed)
        Shutdown();

    bool isVSynched = !mPlayingDemoBuffer && mVSyncUpdates && !mLastDrawWasEmpty && !mVSyncBroken &&
        (!mIsPhysWindowed || (mIsPhysWindowed && mWaitForVSync && !mSoftVSyncWait));
    double aFrameFTime;
    double anUpdatesPerUpdateF;

    if (mVSyncUpdates)
    {
        aFrameFTime = (1000.0 / mSyncRefreshRate) / mUpdateMultiplier;
        anUpdatesPerUpdateF = (float) (1000.0 / (mFrameTime * mSyncRefreshRate));
    }
    else
    {
        aFrameFTime = mFrameTime / mUpdateMultiplier;
        anUpdatesPerUpdateF = 1.0;
    }

    // Do we need to fast forward?
    if (mPlayingDemoBuffer)
    {
        if (mUpdateCount < mFastForwardToUpdateNum || mFastForwardToMarker)
        {
            if (!mDemoMute && !mFastForwardStep)
            {
                mDemoMute = true;
                Mute(true);
            }

            static DWORD aTick = GetTickCount();
            while (mUpdateCount < mFastForwardToUpdateNum || mFastForwardToMarker)
            {
                ClearUpdateBacklog();
                int aLastUpdateCount = mUpdateCount;

                // Actual updating code below
                //////////////////////////////////////////////////////////////////////////

                bool hadRealUpdate = DoUpdateFrames();

                if (hadRealUpdate)
                {
                    mPendingUpdatesAcc += anUpdatesPerUpdateF;
                    mPendingUpdatesAcc -= 1.0;
                    ProcessSafeDeleteList();

                    // Process any extra updates
                    while (mPendingUpdatesAcc >= 1.0)
                    {
                        // These should just be IDLE commands we have to clear out
                        ProcessDemo();

                        bool hasRealUpdate = DoUpdateFrames();
                        DBG_ASSERTE(hasRealUpdate);

                        if (!hasRealUpdate)
                            break;

                        ProcessSafeDeleteList();
                        mPendingUpdatesAcc -= 1.0;
                    }

                    DoUpdateFramesF((float) anUpdatesPerUpdateF);
                    ProcessSafeDeleteList();
                }

                //////////////////////////////////////////////////////////////////////////

                // If the update count doesn't change, its because we are
                //  playing back a demo and need to read more
                if (aLastUpdateCount == mUpdateCount)
                    return true;

                DWORD aNewTick = GetTickCount();
                if (aNewTick - aTick >= 1000 || mFastForwardStep) // let the app draw some
                {
                    mFastForwardStep = false;
                    aTick = GetTickCount();
                    DrawDirtyStuff();
                    return true;
                }
            }
        }

        if (mDemoMute)
        {
            mDemoMute = false;
            mSoundManager->StopAllSounds();
            Unmute(true);
        }
    }

    // Make sure we're not paused
    if (!mPaused && mUpdateMultiplier > 0)
    {
        ulong aStartTime = timeGetTime();

        ulong aCurTime = aStartTime;
        int aCumSleepTime = 0;

        // When we are VSynching, only calculate this FTimeAcc right after drawing

        if (!isVSynched)
            UpdateFTimeAcc();

        // mNonDrawCount is used to make sure we draw the screen at least
        // 10 times per second, even if it means we have to slow down
        // the updates to make it draw 10 times per second in "game time"

        bool didUpdate = false;

        if (mUpdateAppState == UPDATESTATE_PROCESS_1)
		{
			if (++mNonDrawCount < (int) ceil(mMaxNonDrawCount * mUpdateMultiplier) || !mLoaded)
			{
				bool doUpdate = false;

				if (isVSynched)
				{
					// Synch'ed to vertical refresh, so update as soon as possible after draw
					doUpdate = (!mHasPendingDraw) || (mUpdateFTimeAcc >= (int) (aFrameFTime * 0.75));
				}
				else if (mUpdateFTimeAcc >= aFrameFTime)
				{
					doUpdate = true;
				}

				if (doUpdate)
				{
					// Do VSyncBroken test.  This test fails if we're in fullscreen and
					// "don't vsync" has been forced in Advanced settings up Display Properties
					if ((!mPlayingDemoBuffer) && (mUpdateMultiplier == 1.0))
					{
						mVSyncBrokenTestUpdates++;
						if (mVSyncBrokenTestUpdates >= (DWORD) ((1000+mFrameTime-1)/mFrameTime))
						{
							// It has to be running 33% fast to be "broken" (25% = 1/0.800)
							if (aStartTime - mVSyncBrokenTestStartTick <= 800)
							{
								// The test has to fail 3 times in a row before we decide that
								//  vsync is broken overall
								mVSyncBrokenCount++;
								if (mVSyncBrokenCount >= 3)
									mVSyncBroken = true;
							}
							else
								mVSyncBrokenCount = 0;

							mVSyncBrokenTestStartTick = aStartTime;
							mVSyncBrokenTestUpdates = 0;
						}
					}

					bool hadRealUpdate = DoUpdateFrames();
					if (hadRealUpdate)
						mUpdateAppState = UPDATESTATE_PROCESS_2;

					mHasPendingDraw = true;
					didUpdate = true;
				}
			}
		}
		else if (mUpdateAppState == UPDATESTATE_PROCESS_2)
		{
			mUpdateAppState = UPDATESTATE_PROCESS_DONE;

			mPendingUpdatesAcc += anUpdatesPerUpdateF;
			mPendingUpdatesAcc -= 1.0;
			ProcessSafeDeleteList();

			// Process any extra updates
			while (mPendingUpdatesAcc >= 1.0)
			{
				// These should just be IDLE commands we have to clear out
				ProcessDemo();

				++mNonDrawCount;
				bool hasRealUpdate = DoUpdateFrames();
				DBG_ASSERTE(hasRealUpdate);

				if (!hasRealUpdate)
					break;

				ProcessSafeDeleteList();
				mPendingUpdatesAcc -= 1.0;
			}

			//aNumCalls++;
			DoUpdateFramesF((float) anUpdatesPerUpdateF);
			ProcessSafeDeleteList();

			// Don't let mUpdateFTimeAcc dip below 0
			//  Subtract an extra 0.2ms, because sometimes refresh rates have some
			//  fractional component that gets truncated, and it's better to take off
			//  too much to keep our timing tending toward occurring right after
			//  redraws
			if (isVSynched)
				mUpdateFTimeAcc = max(mUpdateFTimeAcc - aFrameFTime - 0.2f, 0.0);
			else
				mUpdateFTimeAcc -= aFrameFTime;

			if (mRelaxUpdateBacklogCount > 0)
				mUpdateFTimeAcc = 0;

			didUpdate = true;
		}

		if (!didUpdate)
		{
			mUpdateAppState = UPDATESTATE_PROCESS_DONE;

			mNonDrawCount = 0;

			if (mHasPendingDraw)
			{
				DrawDirtyStuff();
			}
			else
			{
				// Let us take into account the time it took to draw dirty stuff
				int aTimeToNextFrame = (int) (aFrameFTime - mUpdateFTimeAcc);
				if (aTimeToNextFrame > 0)
				{
					if (!allowSleep)
						return false;

					// Wait till next processing cycle
					++mSleepCount;
					Sleep(aTimeToNextFrame);

					aCumSleepTime += aTimeToNextFrame;
				}
			}
		}

		if (mYieldMainThread)
		{
			// This is to make sure that the title screen doesn't take up any more than
			// 1/3 of the processor time

			ulong anEndTime = timeGetTime();
			int anElapsedTime = (anEndTime - aStartTime) - aCumSleepTime;
			int aLoadingYieldSleepTime = min(250, (anElapsedTime * 2) - aCumSleepTime);

			if (aLoadingYieldSleepTime >= 0)
			{
				if (!allowSleep)
					return false;

				Sleep(aLoadingYieldSleepTime);
			}
		}
    }

    ProcessSafeDeleteList();
    return true;
}

void SexyAppBase::UpdateFrames()
{
    mUpdateCount++;

    if (!mMinimized)
    {
        PerfTimer perfTimer;
        if (mShowFPS && mShowFPSMode <= FPS_ShowCoords)
            perfTimer.Start();

        if (mWidgetManager->UpdateFrame())
            ++mFPSDirtyCount;

        if (mShowFPS && mShowFPSMode <= FPS_ShowCoords)
        {
            mVFPSUpdateCount++;
            mVFPSUpdateTimes += perfTimer.GetDuration();
        }
    }

    mMusicInterface->Update();
    CleanSharedImages();
}

void SexyAppBase::DoUpdateFramesF(float theFrac)
{
    if (mVSyncUpdates && !mMinimized)
        mWidgetManager->UpdateFrameF(theFrac);
}

bool SexyAppBase::DoUpdateFrames()
{
    SEXY_AUTO_PERF("SexyAppBase::DoUpdateFrames");

    if (gScreenSaverActive)
        return false;

    if (mPlayingDemoBuffer)
    {
        if (mLoadingThreadCompleted && !mLoaded && mDemoLoadingComplete)
        {
            mLoaded = true;
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
            mYieldMainThread = false;
            LoadingThreadCompleted();
        }

		// Hrrm not sure why we check (mUpdateCount != mLastDemoUpdateCnt) here
		if (mLoaded != mDemoLoadingComplete || mUpdateCount != mLastDemoUpdateCnt)
		{
		    UpdateFrames();
		    return true;
		}

        return true;
    }
    else
    {
        if (mLoadingThreadCompleted && !mLoaded)
        {
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
            mLoaded = true;
            mYieldMainThread = false;
            LoadingThreadCompleted();

            if (mRecordingDemoBuffer)
            {
                WriteDemoTimingBlock();
                mDemoBuffer.WriteNumBits(0, 1);
                mDemoBuffer.WriteNumBits(DEMO_LOADING_COMPLETE, 5);
            }
        }

        UpdateFrames();
        return true;
    }
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
	//OutputDebugString("MAKING WINDOW\r\n");

    if (mHWnd != nullptr)
    {
        SetWindowLong(mHWnd, GWL_USERDATA, (LONG) nullptr);
        HWND anOldWindow = mHWnd;
        mHWnd = nullptr;
        DestroyWindow(anOldWindow);
        mWidgetManager->mImage = nullptr;
    }

    // Get the work area of the desktop to allow us to center
    RECT aDesktopRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &aDesktopRect, 0);
    bool wide;
    if (mWidescreenAware && mAllowWindowResize && mOrigScreenHeight > 600)
    {
        wide = true;
    }
    else
    {
        wide = false;
        if (!mFullScreenWindow && (mHeight >= aDesktopRect.bottom - aDesktopRect.top || mWidescreenAware >= aDesktopRect.right - aDesktopRect.left))
        {
            mForceFullScreen = true;
            mIsWindowed = false;
        }
    }

    if (mPlayingDemoBuffer || (mIsWindowed && !mFullScreenWindow))
    {
        DWORD aWindowStyle = WS_CLIPCHILDREN | WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        if (mEnableMaximizeButton)
            aWindowStyle |= WS_MAXIMIZEBOX;

        int width = mWidth;
        int height = mHeight;

        if (mEnableWindowAspect)
            width = mHeight * mWindowAspect;

        if (wide)
        {
            if (mHeight >= mOrigScreenHeight * 0.75)
                height = mOrigScreenHeight * 0.75;

            if (mPreferredHeight != -1)
                height = mPreferredHeight;

            double num = (aDesktopRect.bottom - aDesktopRect.top) * 0.9; // 0.8999999761581421 in decomp

            if (mHeight <= num)
                num = mHeight;

            if (height > num)
                height = num;

            if (mEnableWindowAspect)
            {
                width = height * mWindowAspect;
            }
            else
            {
                width = height * Ratio(mWidth, mHeight);
            }
        }

        RECT aRect;
        aRect.left = 0;
        aRect.top = 0;
        aRect.right = width;
        aRect.bottom = height;

        BOOL worked = AdjustWindowRect(&aRect, aWindowStyle, FALSE);

        int aWidth = aRect.right - aRect.left;
        int aHeight = aRect.bottom - aRect.top;

        int aPlaceX = 64;
        int aPlaceY = 64;

        if (mPreferredX != -1)
        {
            aPlaceX = mPreferredX;
            aPlaceY = mPreferredY;

            int aSpacing = 4;

            if (aPlaceX < aDesktopRect.left + aSpacing)
                aPlaceX = aDesktopRect.left + aSpacing;

            if (aPlaceY < aDesktopRect.top + aSpacing)
                aPlaceY = aDesktopRect.top + aSpacing;

            if (aPlaceX + aWidth >= aDesktopRect.right - aSpacing)
                aPlaceX = aDesktopRect.right - aWidth - aSpacing;

            if (aPlaceY + aHeight >= aDesktopRect.bottom - aSpacing)
                aPlaceY = aDesktopRect.bottom - aHeight - aSpacing;
        }

        if (CheckFor98Mill())
        {
            mHWnd = CreateWindowExA(
                0,
                "MainWindow",
                SexyStringToStringFast(mTitle).c_str(),
                aWindowStyle,
                aPlaceX,
                aPlaceY,
                aWidth,
                aHeight,
                nullptr,
                nullptr,
                gHInstance,
                nullptr);
        }
        else
        {
            mHWnd = CreateWindowEx(
                0,
                _S("MainWindow"),
                mTitle.c_str(),
                aWindowStyle,
                aPlaceX,
                aPlaceY,
                aWidth,
                aHeight,
                nullptr,
                nullptr,
                gHInstance,
                nullptr);
        }

        if (mPreferredX == -1)
        {
            MoveWindow(mHWnd,
                aDesktopRect.left + ((aDesktopRect.right - aDesktopRect.left) - aWidth) / 2,
                aDesktopRect.top + (int) (((aDesktopRect.bottom - aDesktopRect.top) - aHeight) * 0.382),
                aWidth, aHeight, FALSE);
        }

        mIsPhysWindowed = true;
    }
    else
    {
        if (CheckFor98Mill())
        {
            mHWnd = CreateWindowExA(
                WS_EX_TOPMOST,
                "MainWindow",
                SexyStringToStringFast(mTitle).c_str(),
                WS_POPUP | WS_VISIBLE,
                0,
                0,
                mWidth,
                mHeight,
                nullptr,
                nullptr,
                gHInstance,
                nullptr);
        }
        else
        {
            mHWnd = CreateWindowEx(
                WS_EX_TOPMOST,
                _S("MainWindow"),
                mTitle.c_str(),
                WS_POPUP | WS_VISIBLE,
                0,
                0,
                GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN),
                nullptr,
                nullptr,
                gHInstance,
                nullptr);
        }

        mIsPhysWindowed = false;
    }

    /*char aStr[256];
    sprintf(aStr, "HWND: %d\r\n", mHWnd);
    OutputDebugString(aStr);*/

    SetWindowLong(mHWnd, GWL_USERDATA, (LONG) this);

    if (mDDInterface == nullptr)
    {
        mDDInterface = new DDInterface(this);

        // Enable 3d setting
        bool is3D = false;
        bool is3DOptionSet = RegistryReadBoolean("Is3D", &is3D);
        if (is3DOptionSet)
        {
            if (mAutoEnable3D)
            {
                mAutoEnable3D = false;
                mTest3D = true;
            }

            if (is3D)
                mTest3D = true;

            mDDInterface->mIs3D = is3D;
        }
    }

    int aResult = InitDDInterface();
    if (mIsWindowed && aResult == DDInterface::RESULT_INVALID_COLORDEPTH)
    {
        if (mForceWindowed)
        {
            Popup(GetString("PLEASE_SET_COLOR_DEPTH", _S("Please set your desktop color depth to 16 bit.")));
            DoExit(1);
        }
        else
        {
            mForceFullScreen = true;
            SwitchScreenMode(false);
        }

        return;
    }
    else if (!mIsWindowed && (aResult == DDInterface::RESULT_EXCLUSIVE_FAIL ||
                              aResult == DDInterface::RESULT_DISPCHANGE_FAIL))
    {
        mForceWindowed = true;
        SwitchScreenMode(true);
    }
    else if (aResult == DDInterface::RESULT_3D_FAIL)
    {
        Set3DAccelerated(false);
        return;
    }
    else if (aResult != DDInterface::RESULT_OK)
    {
        if (Is3DAccelerated())
        {
            Set3DAccelerated(false);
            return;
        }
        else
        {
            Popup(GetString("FAILED_INIT_DIRECTDRAW", _S("Failed to initialize DirectDraw: ")) + StringToSexyString(DDInterface::ResultToString(aResult) + " " + mDDInterface->mErrorString));
            DoExit(1);
        }
    }

    if (!Is3DAccelerated() && mIsWindowed)
    {
        RECT clientRect;
        GetClientRect(mHWnd, &clientRect);

        int newWidth = mWidgetManager->mWidth;
        int newHeight = mWidgetManager->mHeight;

        if (mEnableWindowAspect)
            newWidth = newHeight * mWindowAspect;

        if (clientRect.right != newWidth || clientRect.bottom != newHeight)
        {
            if (newHeight >= aDesktopRect.bottom - aDesktopRect.top)
            {
                mForceFullScreen = true;
                SwitchScreenMode(false);
                return;
            }

            int newX = 64;
            int newY = 64;
            if (mPreferredX != -1)
            {
                int aSpacing = 4;

                if (aDesktopRect.left + aSpacing > 64)
                    newX = aDesktopRect.left + aSpacing;

                if (aDesktopRect.top + aSpacing > 64)
                    newY = aDesktopRect.top + aSpacing;

                if (newX + newWidth >= aDesktopRect.right - aSpacing)
                    newX = aDesktopRect.right - newWidth - aSpacing;

                if (newY + newHeight >= aDesktopRect.bottom - aSpacing)
                    newY = aDesktopRect.bottom - newHeight - aSpacing;
            }

            MoveWindow(mHWnd, newX, newY, newWidth, newHeight, FALSE);
        }
    }

    bool isActive = mActive;
    mActive = GetActiveWindow() == mHWnd;

    mPhysMinimized = false;
    if (mMinimized)
    {
        if (mMuteOnLostFocus)
            Unmute(true);

        mMinimized = false;
        isActive = mActive; // set this here so we don't call RehupFocus again.
        RehupFocus();
    }

    if (isActive != mActive)
        RehupFocus();

    ReInitImages();

    mWidgetManager->mImage = mDDInterface->GetScreenImage();
    mWidgetManager->MarkAllDirty();

    SetTimer(mHWnd, 100, mFrameTime, nullptr);
}

void SexyAppBase::EnforceCursor()
{
    // TODO: Implement
}

void SexyAppBase::ReInitImages()
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

void SexyAppBase::Mute(bool autoMute)
{
    // TODO: Implement
}

void SexyAppBase::Unmute(bool autoMute)
{
    // TODO: Implement
}

void SexyAppBase::LoadingThreadProc() {}
void SexyAppBase::LoadingThreadCompleted() {}

void SexyAppBase::LoadingThreadProcStub(void *theArg)
{
    SexyAppBase* aSexyApp = (SexyAppBase*) theArg;

    aSexyApp->LoadingThreadProc();

    char aStr[256];
    sprintf(aStr, "Resource Loading Time: %d\r\n", (GetTickCount() - aSexyApp->mTimeLoaded));
    OutputDebugStringA(aStr);

    aSexyApp->mLoadingThreadCompleted = true;
}

void SexyAppBase::StartLoadingThread()
{
    if (!mLoadingThreadStarted)
    {
        mYieldMainThread = true;
        ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
        mLoadingThreadStarted = true;
        _beginthread(LoadingThreadProcStub, 0, this);
    }
}

bool SexyAppBase::DrawDirtyStuff()
{
    // TODO: Implement
    return false;
}

void SexyAppBase::Redraw(Rect *theClipRect)
{
    // TODO: Implement
}

bool SexyAppBase::CheckSignature(const Buffer& theBuffer, const std::string& theFileName)
{
    // Add your own signature checking code here
    return false;
}

bool SexyAppBase::LoadProperties(const std::string& theFileName, bool required, bool checkSig)
{
    Buffer aBuffer;
    if (!ReadBufferFromFile(theFileName, &aBuffer))
    {
        if (!required)
            return true;
        else
        {
            Popup(GetString("UNABLE_OPEN_PROPERTIES", _S("Unable to open properties file ")) + StringToSexyString(theFileName));
            return false;
        }
    }
    if (checkSig)
    {
        if (!CheckSignature(aBuffer, theFileName))
        {
            Popup(GetString("PROPERTIES_SIG_FAILED", _S("Signature check failed on ")) + StringToSexyString(theFileName + "'"));
            return false;
        }
    }

    PropertiesParser aPropertiesParser(this);

    // Load required language-file properties
    if (!aPropertiesParser.ParsePropertiesBuffer(aBuffer))
    {
        Popup(aPropertiesParser.GetErrorText());
        return false;
    }
    else
        return true;
}

bool SexyAppBase::LoadProperties()
{
    // Load required language-file properties
    return LoadProperties("properties\\default.xml", true, false);
}

void SexyAppBase::InitPropertiesHook() {}

bool SexyAppBase::ReadBufferFromFile(const std::string& theFileName, Buffer* theBuffer, bool dontWriteToDemo)
{
    // TODO: Implement
    return false;
}

void SexyAppBase::SetInteger(const std::string &theId, int theValue)
{
    std::pair<StringIntMap::iterator, bool> aPair = mIntProperties.insert(StringIntMap::value_type(theId, theValue));
    if (!aPair.second) // Found it, change value
        aPair.first->second = theValue;
}

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

bool SexyAppBase::GetBoolean(const std::string& theId)
{
    StringBoolMap::iterator anItr = mBoolProperties.find(theId);
    DBG_ASSERTE(anItr != mBoolProperties.end());

    if (anItr != mBoolProperties.end())
        return anItr->second;
    else
        return false;
}

bool SexyAppBase::GetBoolean(const std::string& theId, bool theDefault)
{
    StringBoolMap::iterator anItr = mBoolProperties.find(theId);

    if (anItr != mBoolProperties.end())
        return anItr->second;
    else
        return theDefault;
}

int SexyAppBase::GetInteger(const std::string& theId)
{
    StringIntMap::iterator anItr = mIntProperties.find(theId);
    DBG_ASSERTE(anItr != mIntProperties.end());

    if (anItr != mIntProperties.end())
        return anItr->second;
    else
        return 0;
}

int SexyAppBase::GetInteger(const std::string& theId, int theDefault)
{
    StringIntMap::iterator anItr = mIntProperties.find(theId);

    if (anItr != mIntProperties.end())
        return anItr->second;
    else
        return theDefault;
}

double SexyAppBase::GetDouble(const std::string& theId)
{
    StringDoubleMap::iterator anItr = mDoubleProperties.find(theId);
    DBG_ASSERTE(anItr != mDoubleProperties.end());

    if (anItr != mDoubleProperties.end())
        return anItr->second;
    else
        return false;
}

double SexyAppBase::GetDouble(const std::string& theId, double theDefault)
{
    StringDoubleMap::iterator anItr = mDoubleProperties.find(theId);

    if (anItr != mDoubleProperties.end())
        return anItr->second;
    else
        return theDefault;
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

bool SexyAppBase::RegistryReadBoolean(const std::string& theKey, bool* theValue)
{
    // TODO: Implement
    return false;
}

bool SexyAppBase::PrepareDemoCommand(bool required)
{
    // TODO: Implement
    return false;
}

void SexyAppBase::WriteDemoTimingBlock()
{
    // TODO: Implement
}

bool SexyAppBase::ReadDemoBuffer(std::string &theError)
{
    // TODO: Implement
    return false;
}

bool SexyAppBase::Is3DAccelerated()
{
    return mDDInterface->mIs3D;
}

void SexyAppBase::Set3DAccelerated(bool is3D, bool reinit)
{
    // TODO: Implement
}

void SexyAppBase::SwitchScreenMode()
{
	SwitchScreenMode(mIsWindowed, Is3DAccelerated(), true);
}

void SexyAppBase::SwitchScreenMode(bool wantWindowed)
{
	SwitchScreenMode(wantWindowed, Is3DAccelerated());
}

void SexyAppBase::SwitchScreenMode(bool wantWindowed, bool is3d, bool force)
{
    // TODO: Implement
}

int SexyAppBase::InitDDInterface()
{
    // TODO: Implement
    return DDInterface::EResult::RESULT_OK;
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

bool SexyAppBase::AppCanRestore()
{
    return !mIsDisabled;
}

static int DemoJumpToTime()
{
    // TODO: Implement
    return 0;
}

static int ListDemoMarkers()
{
    // TODO: Implement
    return 0;
}

static void ToggleDemoSoundVolume()
{
    // TODO: Implement
}

static DWORD gPowerSaveTick = 0;
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

    bool v9;
    WPARAM v10;
    if (uMsg > WM_SYSKEYUP)
    {
        if (uMsg > WM_MBUTTONUP)
        {
            if (uMsg != WM_MOUSEWHEEL && uMsg != WM_SIZING)
            {
                if (uMsg == WM_POWERBROADCAST && wParam <= 1)
                {
                    if (aSexyApp != nullptr)
                    {
                        if (!aSexyApp->mAllowMonitorPowersave || !aSexyApp->mLoaded || !aSexyApp->mIsPhysWindowed)
                            return BROADCAST_QUERY_DENY;
                        goto LABEL_12;
                    }
                    goto LABEL_201;
                }
                goto LABEL_12;
            }
        }
        else if (uMsg < WM_MOUSEMOVE)
        {
            if (uMsg == WM_SYSCHAR)
            {
                if (aSexyApp != nullptr)
                {
                    if (aSexyApp->IsAltKeyUsed(wParam))
                        return 0;
                    goto LABEL_12;
                }
                goto LABEL_201;
            }

            if (uMsg == WM_SYSCOMMAND)
            {
                WPARAM newWParam = wParam & 0xFFF0;
                if (newWParam != SC_MONITORPOWER
                    || (gPowerSaveTick = GetTickCount(), aSexyApp == nullptr)
                    || aSexyApp->mAllowMonitorPowersave && aSexyApp->mLoaded)
                {
                    if (newWParam != SC_SCREENSAVE)
                        goto LABEL_12;
                    if (aSexyApp != nullptr)
                    {
                        if (!aSexyApp->mLoaded || !aSexyApp->mIsPhysWindowed)
                            return 0;
                        goto LABEL_12;
                    }
                    goto LABEL_201;
                }
                return 0;
            }
            if (uMsg != WM_TIMER)
                goto LABEL_12;
        }

LABEL_34:
        if (aSexyApp == nullptr)
            goto LABEL_201;
        if (aSexyApp->mNoDefer)
            goto LABEL_12;
        if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
        {
            v9 = true;
        }
        else
        {
            v9 = false;
            if (uMsg != WM_KEYUP && uMsg != WM_SYSKEYUP)
            {
                v10 = wParam;
                goto LABEL_106;
            }
        }

        v10 = wParam;
        if (wParam == VK_CONTROL)
        {
            aSexyApp->mCtrlDown = v9;
        }
        else if (wParam == VK_MENU)
        {
            aSexyApp->mAltDown = v9;
        }

        if (v9 && aSexyApp->DebugKeyDownAsync(wParam, aSexyApp->mCtrlDown, aSexyApp->mAltDown))
        {
            return 0;
        }

LABEL_106:
        if (aSexyApp->mPlayingDemoBuffer)
        {
            if (uMsg == WM_CHAR)
            {
                int v25;
                int v26;
                switch (v10)
                {
                    case '+':
                        aSexyApp->mUpdateMultiplier *= 1.5;
                        break;
                    case '-':
                        aSexyApp->mUpdateMultiplier /= 1.5;
                        break;
                    case '0':
                        aSexyApp->mFastForwardToUpdateNum = aSexyApp->mDemoLength;
                        break;
                    case '4':
                        v25 = 120000;
                        goto LABEL_119;
                    case '5':
                        v26 = 90000;
                        goto LABEL_121;
                    case '6':
                        v25 = 60000;
                        goto LABEL_119;
                    case '7':
                        v26 = 30000;
                        goto LABEL_121;
                    case '8':
                        v25 = 10000;
LABEL_119:
                        aSexyApp->mFastForwardToUpdateNum = aSexyApp->mDemoLength - v25 / aSexyApp->mFrameTime;
                        break;
                    case '9':
                        v26 = 5000;
LABEL_121:
                        aSexyApp->mFastForwardToUpdateNum = aSexyApp->mDemoLength - v26 / aSexyApp->mFrameTime;
                        break;
                    case '=':
                        aSexyApp->mUpdateMultiplier = 1.0;
                        break;
                    case 'J':
                    case 'j':
                        DemoJumpToTime();
                        break;
                    case 'L':
                    case 'l':
                        ListDemoMarkers();
                        break;
                    case 'M':
                    case 'm':
                        aSexyApp->mFastForwardToMarker = true;
                        break;
                    case 'N':
                    case 'n':
                        aSexyApp->mFastForwardToUpdateNum = aSexyApp->mUpdateCount + 1;
                        aSexyApp->mFastForwardStep = true;
                        break;
                    case 'P':
                    case 'p':
                        aSexyApp->mPaused = !aSexyApp->mPaused;
                        aSexyApp->mUpdateFTimeAcc = 0.0;
                        aSexyApp->mLastTimeCheck = timeGetTime();
                        break;
                    case 'S':
                    case 's':
                        ToggleDemoSoundVolume();
                        break;
                    default:
                        break;
                }
            }
            else if (uMsg == WM_KEYDOWN)
            {
                aSexyApp->DebugKeyDown(v10);
            }
        }

        bool pushMessage = true;
        if (!aSexyApp->mDeferredMessages.empty())
        {
            // Don't add any more messages after WM_CLOSE
            MSG* aMsg = &aSexyApp->mDeferredMessages.back();

            if (aMsg->message == WM_CLOSE)
                pushMessage = false;
            if (uMsg == WM_TIMER && uMsg == aMsg->message)
                goto LABEL_12;
            if (!pushMessage)
            {
LABEL_153:
                if (uMsg == WM_WINDOWPOSCHANGING)
                {
                    RECT winRect;
                    RECT clientRect;
                    GetWindowRect(hWnd, &winRect);
                    GetClientRect(hWnd, &clientRect);
                    LPWINDOWPOS winPos = (LPWINDOWPOS) lParam;
                    int winPosLeft = winPos->x;
                    int winPosTop = winPos->y;
                    int winPosRight = winPosLeft + winPos->cx;
                    int winPosBottom = winPosTop + winPos->cy;
                    int winWidth = winPosRight - winPosLeft;
                    int winHeight = winPosBottom - winPosTop;
                    int winPosX = winPosLeft;
                    int winPosY = winPosTop;
                    double double1 = (double)(winRect.right - winRect.left) / (double)(winRect.bottom - winRect.top);
                    double double2 = (double)(winPosRight - winPosLeft) / (double)(winPosBottom - winPosTop);

                    Rect rect1;
                    rect1.mX = 0;
                    rect1.mY = 0;
                    rect1.mWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                    rect1.mHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

                    Rect rect2;
                    rect2.mX = winRect.left;
                    rect2.mY = winRect.top;
                    rect2.mHeight = winRect.bottom - winRect.top;
                    rect2.mWidth = winRect.right - winRect.left;

                    bool intersects = false;
                    if (rect1.Intersects(rect2))
                    {
                        rect2.mX = winPosX;
                        rect2.mWidth = winWidth;
                        rect2.mHeight = winHeight;
                        rect2.mY = winPosY;
                        if (rect1.Intersects(rect2))
                            intersects = true;
                    }

                    double double3 = double1 - double2;
                    double double4 = fabs(double3);
                    if (double4 > 0.01f && intersects)
                    {
                        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
                        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
                        int virtualScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                        Ratio ratio;
                        if (aSexyApp->mEnableWindowAspect)
                            ratio = aSexyApp->mWindowAspect;
                        else
                            ratio.Set(aSexyApp->mWidth, aSexyApp->mHeight);

                        int horiz = clientRect.right + winPosRight + winRect.left - winRect.right - winPosX;
                        int vert = clientRect.bottom + winPosBottom + winRect.top - winRect.bottom - winPosY;
                        int num1 = vert * ratio;
                        int num2 = horiz;
                        int num3 = horiz;
                        if (horiz <= num1)
                            num3 = vert * ratio;
                        if (num3 >= winHeight)
                        {
                            num2 = winHeight;
                        }
                        else if (horiz <= num1)
                        {
                            num2 = vert * ratio;
                            winWidth = num2;
LABEL_170:
                            int num4;
                            int num5 = num2 * ratio;
                            if (num5 <= screenHeight)
                            {
                                num4 = winWidth;
                            }
                            else
                            {
                                num4 = screenHeight * ratio;
                                num5 = screenHeight;
                            }

                            int num6 = num5 - vert + winPosBottom;
                            int num7 = winPos->x;
                            int num8 = num4 - horiz + winPosRight - winPosX;
                            winPos->cy = num6 - winPosY;
                            winPos->cx = num8;
                            if (num8 + num7 > virtualScreenWidth)
                                winPos->x = virtualScreenWidth - num8;

                            goto LABEL_12;
                        }
                        winWidth = num2;
                        goto LABEL_170;
                    }
LABEL_12:
                    if (aSexyApp != nullptr && uMsg == aSexyApp->mNotifyGameMessage && hWnd == aSexyApp->mHWnd)
                    {
                        aSexyApp->HandleNotifyGameMessage(wParam, lParam);
                    }
                    goto LABEL_201;
                }
                if (uMsg != WM_SIZING)
                {
                    switch (uMsg)
                    {
                        case WM_SIZE:
                            aSexyApp->mPhysMinimized = wParam == SIZE_MINIMIZED;
                            if (wParam == SIZE_RESTORED && aSexyApp->mWidescreenAware)
                            {
                                RECT clientRect;
                                GetClientRect(hWnd, &clientRect);
                                aSexyApp->mDDInterface->WindowResize(clientRect.right, clientRect.bottom);

                                Rect presRect = aSexyApp->mDDInterface->mPresentationRect;

                                aSexyApp->mWidgetManager->mMouseSourceRect.mX = presRect.mX;
                                aSexyApp->mWidgetManager->mMouseSourceRect.mY = presRect.mY;
                                aSexyApp->mWidgetManager->mMouseSourceRect.mWidth = presRect.mWidth;
                                aSexyApp->mWidgetManager->mMouseSourceRect.mHeight = presRect.mHeight;

                                InvalidateRect(hWnd, nullptr, false);
                            }
                            break;
                        case WM_SYSKEYDOWN:
                            if (wParam != VK_F4)
                                return 0;
                            break;
                        case WM_CLOSE:
                            aSexyApp->CloseRequestAsync();
                            return 0;
                        default:
                            break;
                    }

                    goto LABEL_12;
                }

                if (!aSexyApp->mWidescreenAware)
                    goto LABEL_12;

                RECT wndRect;
                GetWindowRect(hWnd, &wndRect);
                LPRECT theRect = (LPRECT)lParam;
                if (wParam != WMSZ_BOTTOMRIGHT)
                {
                    theRect->left = wndRect.left;
                    theRect->right = wndRect.right;
                    theRect->top = wndRect.top;
                    theRect->bottom = wndRect.bottom;
                    return 0;
                }

                RECT cRect;
                GetClientRect(hWnd, &cRect);
                Ratio ratio;
                if (aSexyApp->mEnableWindowAspect)
                    ratio = aSexyApp->mWindowAspect;
                else
                    ratio.Set(aSexyApp->mWidth, aSexyApp->mHeight);

                int height = theRect->bottom - theRect->top;
                cRect.right += wndRect.left + theRect->right - theRect->left - wndRect.right;
                cRect.bottom += wndRect.top + height - wndRect.bottom;
                int vNum = cRect.right;
                if (cRect.right <= cRect.bottom * ratio)
                    vNum = cRect.bottom * ratio;
                int n1;
                int v59 = cRect.right;
                if (vNum >= GetSystemMetrics(SM_CXSCREEN))
                {
                    n1 = GetSystemMetrics(SM_CXSCREEN);
                }
                else
                {
                    n1 = cRect.bottom * ratio;
                    if (cRect.right > n1)
                    {
LABEL_188:
                        int v60 = v59 * ratio;
                        if (v60 > GetSystemMetrics(SM_CYSCREEN))
                        {
                            v60 = GetSystemMetrics(SM_CYSCREEN);
                            v59 = v60 * ratio;
                        }

                        theRect->right += v59 - cRect.right;
                        theRect->bottom += v60 - cRect.bottom;
                        goto LABEL_12;
                    }
                }

                v59 = n1;
                goto LABEL_188;
            }

            if (uMsg == WM_SYSCOLORCHANGE || uMsg == WM_DISPLAYCHANGE)
            {
                WindowsMessageList::iterator aMsgListItr = aSexyApp->mDeferredMessages.begin();
                while (true)
                {
                    if (aMsgListItr == aSexyApp->mDeferredMessages.end())
                        break;

                    MSG& aMsg2 = *aMsgListItr;

                    if (aMsg2.message == WM_SYSCOLORCHANGE || aMsg2.message == WM_DISPLAYCHANGE)
                        pushMessage = false;

                    ++aMsgListItr;

                    if (!pushMessage)
                        goto LABEL_153;
                }
            }
        }

        MSG msg;
        msg.hwnd = hWnd;
        msg.message = uMsg;
        msg.lParam = lParam;
        msg.wParam = wParam;

        aSexyApp->mDeferredMessages.push_back(msg);

        goto LABEL_153;
    }

    LRESULT result;

    if (uMsg >= WM_SYSKEYDOWN)
        goto LABEL_34;
    if (uMsg <= WM_ENDSESSION)
    {
        if (uMsg != WM_ENDSESSION)
        {
            switch (uMsg)
            {
                case WM_DESTROY:
                {
                    char aStr[256];
                    sprintf(aStr, "DESTROYED HWND: %d\r\n", hWnd);
                    OutputDebugStringA(aStr);
                    goto LABEL_12;
                }
                case WM_MOVE:
                case WM_SIZE:
                case WM_CLOSE:
                case WM_SYSCOLORCHANGE:
                    goto LABEL_34;
                case WM_ENABLE:
                    if (aSexyApp == nullptr)
                        goto LABEL_201;
                    aSexyApp->mIsDisabled = wParam == 0;
                    goto LABEL_12;
                case WM_PAINT:
                {
                    if (aSexyApp == nullptr)
                        goto LABEL_201;
                    if (!aSexyApp->mInitialized || gInAssert || aSexyApp->mSEHOccured)
                        goto LABEL_12;
                    RECT newClientRect;
                    GetClientRect(hWnd, &newClientRect);

                    PAINTSTRUCT ps;
                    BeginPaint(hWnd, &ps);

                    if (aSexyApp->mRunning)
                        aSexyApp->Redraw(nullptr);

                    EndPaint(hWnd, &ps);

                    result = 0;
                    break;
                }
                case WM_QUERYOPEN:
                    if (aSexyApp == nullptr)
                        goto LABEL_201;
                    if (aSexyApp->AppCanRestore())
                        goto LABEL_12;
                    result = 0;
                    break;
                case WM_ERASEBKGND:
                    return 1;
                default:
                    goto LABEL_12;
            }

            return result;
        }

        if (aSexyApp != nullptr)
        {
            aSexyApp->Shutdown();
            goto LABEL_12;
        }
LABEL_201:
        if (gSexyAppBase->mIsWideWindow)
            return DefWindowProcW(hWnd, uMsg, wParam, lParam);
        else
            return DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
        case WM_ACTIVATEAPP:
            if (aSexyApp == nullptr)
                goto LABEL_201;
            if (!aSexyApp->mPlayingDemoBuffer && hWnd == aSexyApp->mHWnd)
                aSexyApp->mActive = wParam != 0;
            goto LABEL_34;
        case WM_SETCURSOR:
            if (aSexyApp != nullptr && !aSexyApp->mSEHOccured)
                aSexyApp->EnforceCursor();
            return 1;
        case WM_GETMINMAXINFO:
        {
            if (aSexyApp != nullptr)
            {
                RECT nRect;
                nRect.left = 0;
                nRect.top = 0;
                nRect.right = aSexyApp->mWidgetManager->mWidth;
                nRect.bottom = aSexyApp->mWidgetManager->mHeight;
                if (aSexyApp->mEnableWindowAspect)
                    nRect.right = aSexyApp->mWidgetManager->mHeight * aSexyApp->mWindowAspect;

                DWORD windowStyle = GetWindowLongA(hWnd, GWL_STYLE);
                AdjustWindowRect(&nRect, windowStyle, FALSE);

                LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
                lpMMI->ptMaxSize.x = nRect.right - nRect.left;
                lpMMI->ptMaxTrackSize.x = nRect.right - nRect.left;
                lpMMI->ptMaxSize.y = nRect.bottom - nRect.top;
                lpMMI->ptMaxTrackSize.y = nRect.bottom - nRect.top;

                if (!gIs3D && aSexyApp->mDDInterface->mInitialized)
                {
                    lpMMI->ptMinTrackSize.x = lpMMI->ptMaxTrackSize.x;
                    lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y;
                }
            }
            return 0;
        }
        case WM_WINDOWPOSCHANGING:
        case WM_DISPLAYCHANGE:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
            goto LABEL_34;
        case WM_NCHITTEST:
        {
            if (aSexyApp == nullptr)
                goto LABEL_201;

            if ((GetWindowLongA(hWnd, GWL_STYLE) & WS_THICKFRAME) == 0)
                goto LABEL_12;

            RECT aRect;
            GetClientRect(hWnd, &aRect);
            POINT p;
            ClientToScreen(hWnd, &p);
            aRect.right = p.x;
            aRect.bottom = p.y;

            int curX = LOWORD(lParam);
            int curY = HIWORD(lParam);

            int someNum1 = p.x + p.y - curY - curX;
            int someNum2 = (aRect.right - aRect.left) / 40;
            if (gIs3D && someNum1 < someNum2)
            {
                aSexyApp->mIsSizeCursor = true;
                ::SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
                result = HTBOTTOMRIGHT;
            }
            else
            {
                if (gSexyAppBase->mIsWideWindow)
                    result = DefWindowProcW(hWnd, uMsg, wParam, lParam);
                else
                    result = DefWindowProcA(hWnd, uMsg, wParam, lParam);

                if (result == HTTOP)
                {
                    ::SetCursor(LoadCursor(nullptr, IDC_ARROW));
                    result = HTCAPTION;
                }
                else if (result == HTLEFT || result == HTRIGHT || result == HTBOTTOM)
                {
                    ::SetCursor(LoadCursor(nullptr, IDC_ARROW));
                    result = HTBORDER;
                }
                else
                {
                    aSexyApp->mIsSizeCursor = false;
                }
            }

            break;
        }
        case WM_NCLBUTTONDOWN:
        {
            if (aSexyApp == nullptr)
                goto LABEL_201;

            LRESULT ret = DefWindowProcA(hWnd, uMsg, wParam, lParam);
            if (aSexyApp->mPauseWhenMoving)
                aSexyApp->ClearUpdateBacklog();
            return ret;
        }
        default:
            goto LABEL_12;
    }

    return result;
}
