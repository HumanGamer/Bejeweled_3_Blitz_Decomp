#include "Bej3App.h"

#include <CfgMachine.h>
#include <D3DTester.h>
#include "GraphicsRecorder.h"
#include "PopDRMComm.h"
#include "UnderDialogWidget.h"

using namespace Sexy;

Bej3App* Sexy::gApp;

constexpr int OFFLINE_BOOST_COSTS[NUM_BOOSTS]
{
    3000, 4000, 3000, 2000, 2000
};

constexpr int ONLINE_BOOST_COSTS[NUM_BOOSTS]
{
    5000, 7500, 5500, 2500, 3500
};

Bej3App::Bej3App()
{
    mLogDebugViewToFile = false;
    if (GetAsyncKeyState(VK_SHIFT) < 0)
        mLogDebugViewToFile = true;

    if (mLogDebugViewToFile)
        UpdateDebugViewHooks();

    mMaxNonDrawCount = 50;
    mReInit = false;
    mPreInitArtRes = 0;
    mResForced = false;
    mWas3D = false;
    mArtRes = 600;
    mWidescreenAware = true;
    mAllowWindowResize = true;
    mEnableMaximizeButton = true;
    mResChanged = false;
    mCurVoice = nullptr;
    mIgnoreSound = false;
    mPauseFrames = 0;
    mFocusedAfterLoad = false;
    mNoVSync = false;
    mFullscreenBits = 32;
    mJumpToZen = false;
    mJumpToClassic = false;
    mJumpToBlitz = false;
    mLastLocalTime = 0;
    mMaxGamesPerDay = 0;
    mRegCodeNotNeeded = false;
    mWantDataUpdateOnFocus = false;
    mWaitingForRegUpdateTicks = 0;
    mBuyCoinsURL = "http://www.popcap.com";
#ifdef BEJ_BLITZ_109_7400
    mSkipCoinCode = false;
    mUpdater = nullptr;
    mCheckedForUpdates = false;
    mUpdaterVisible = false;
    mUpdaterShutdown = false;
    mCurrentVersion = "";
    mPatchNoteVersion = "";
#endif
    mStatsGetHandle = -1;
    mStatsStalled = false;
    mAutoEnable3D = true;

    mProdName = "BejeweledBlitz";
    mRegKey = "PopCap\\BejeweledBlitz";
    mTitle = StringToSexyString("Bejeweled Blitz v" + mProductVersion);

    mProfile = nullptr;
    mMenu = nullptr;
    mMainMenu = nullptr;
    mBoard = nullptr;
    mLoaderScreen = nullptr;
    mFacebookWidget = nullptr;
#ifndef BEJ_BLITZ_109_7400
    mBoostWidget = nullptr;
#endif
    mCustomBassMusicInterface = nullptr;
    mCurVoice = nullptr;
    mCurVoiceId = -1;
    mNextVoice = nullptr;
    mNextVoiceId = -1;
    mShrunkenGems = nullptr;
    mTempleObject = nullptr;
    mBlitzBackground = nullptr;
    mBlitzBackgroundLo = nullptr;

    gGR = new GraphicsRecorder();

    MEMORYSTATUS memStatus;
    memset(&memStatus, 0, sizeof(memStatus));
    memStatus.dwLength = 32;
    GlobalMemoryStatus(&memStatus);

    uint physMemTotal = memStatus.dwTotalPhys >> 20;
    uint physMemAvailable = memStatus.dwAvailPhys >> 20;
    uint vidMemory = D3DTester::StaticGetVideoMemoryMB();
    if (vidMemory == 0)
        vidMemory = 32; // the check failed, there can't be NO video memory!

    SetInteger("compat_AppVidMemory", vidMemory);
    SetInteger("compat_AppSysMemFreeMB", physMemAvailable);
    SetInteger("compat_AppSysMemTotalMB", physMemTotal);

    SetInteger("compat_AppOrigScreenWidth", mOrigScreenWidth);
    SetInteger("compat_AppOrigScreenHeight", mOrigScreenHeight);

    CfgMachineValue artResValue;
    if (mCompatCfgMachine == nullptr)
        goto LABEL_35;

    int artRes = 0;
    if (mCompatCfgMachine->MachineExecuteFunction("GetAppDefaultArtRes", &artResValue))
    {
        artRes = artResValue.GetInteger();
    }

    if (artRes > 0)
    {
        if (artRes != 1200 && artRes != 768 && artRes != 600)
        {
            Popup("Default art resolution must be either 1200, 768, or 600");
        }

        mArtRes = artRes;
    }
    else
    {
LABEL_35:
        if (physMemTotal < 768 || mOrigScreenHeight < 1200 || vidMemory < 92)
        {
            if (physMemTotal < 256 || mOrigScreenHeight < 600 || vidMemory < 60)
                mArtRes = 600;
            else
                mArtRes = 768;
        }
        else
        {
            mArtRes = 1200;
        }
    }

    mMusicVolume = 0.5;
    mVoiceVolume = 0.85;
#ifdef BEJ_BLITZ_109_7400
    mBoostAutoRenew = true;
#endif
    mHeight = mArtRes;
    mQuitCountdown = 0;
    mTooltipManager = nullptr;
    mWindowedButton = nullptr;
    mResizeWidget = nullptr;
    mTipIdx = 0;
    mHighScores = nullptr;
    mCreatingFBUser = false;
    mWidth = mHeight * Ratio(4, 3);

    for (int i = 0; i < NUM_BOOSTS; i++)
        mBoostCosts[i] = ONLINE_BOOST_COSTS[i];

    mUnderDialogWidget = new UnderDialogWidget();
    mDialogObscurePct = 0.0;

    mWebRoot = "http://labs.popcap.com/facebook/bj3";
    mForcedWebRoot = false;

    mDRMComm = new PopDRMComm();

    // TODO: Could this be an inlined function?
    if (!mDRMComm->mConnected)
    {
        mDRMComm->mEvent = CreateEvent(nullptr, false, false, nullptr);
        PostMessage(HWND_BROADCAST, mDRMComm->mConnectMsg, GetCurrentProcessId(), (LPARAM) mDRMComm->mHwnd);
        WaitForSingleObject(mDRMComm->mEvent, 1u);
        CloseHandle(mDRMComm->mEvent);
        mDRMComm->mEvent = nullptr;
    }
}

Bej3App::~Bej3App()
{
    // TODO: Implement
}

void Bej3App::Init()
{
    SexyApp::Init();

    if (mLogDebugViewToFile)
        UpdateDebugViewHooks();

    if (!mForcedWebRoot)
    {
        mWebRoot = SexyStringToString(GetString("WebRoot", ToSexyString(mWebRoot)));
    }

    mMaxGamesPerDay = GetInteger("MaxGamesPerDay");

    if (!mBuildUnlocked)
    {
        // TODO: Is this an inlined function?
        if (IsWindow(mDRMComm->mDRMHwnd))
        {
            mIsRegistered = SendMessage(mDRMComm->mDRMHwnd, mDRMComm->mQueryMsg, 4, 0) != 0;
        }
        else
        {
            mDRMComm->mConnected = false;
            mIsRegistered = false;
        }
    }

    // TODO: Implement
}

void Sexy::Bej3App::UpdateDebugViewHooks()
{
    // TODO: Implement
}
