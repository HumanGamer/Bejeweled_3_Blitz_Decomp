#include "SexyAppBase.h"

#include <ImageLib.h>
#include <time.h>

#include "CfgMachine.h"
#include "DDInterface.h"
#include "Debug.h"
#include "SEHCatcher.h"
#include "PopLoc.h"
#include "ResourceManager.h"
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

Sexy::SexyAppBase::SexyAppBase()
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

Sexy::SexyAppBase::~SexyAppBase()
{
    // TODO: Implement
}

void Sexy::SexyAppBase::Init()
{
    // TODO: Implement
}

void Sexy::SexyAppBase::Start()
{
    // TODO: Implement
}

void Sexy::SexyAppBase::Shutdown()
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

void SexyAppBase::EnforceCursor()
{
    // TODO: Implement
}

void SexyAppBase::SetMusicVolume(double theVolume)
{
    // TODO: Implement
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
