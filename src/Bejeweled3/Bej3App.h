#pragma once

#include <Common.h>
#include <SexyApp.h>

#define NUM_BOOSTS 5

class PopDRMComm;

namespace Sexy
{
    class SoundInstance;
    class Board;
    class ButtonWidget;
    class CustomBassMusicInterface;
    class CustomSoundManager;
    class Profile;
    class MainMenu;
    class Menu;
    class HighScores;
    class Background;
    class LoaderScreen;
    class FacebookWidget;
    class TooltipManager;
    class BoostWidget;
    class D3DObject;
    class UnderDialogWidget;
    class ResizeWidget;
    class UpdaterWidget;

    class Bej3App : public SexyApp
    {
    public:
        double mVoiceVolume;
#ifdef BEJ_BLITZ_109_7400
        bool mBoostAutoRenew;
#endif
        bool mIgnoreSound;
        std::string mWebRoot;
        bool mForcedWebRoot;
        bool mReInit;
        bool mWas3D;
        bool mResChanged;
        bool mResForced;
        bool mJumpToZen;
        bool mJumpToClassic;
        bool mJumpToBlitz;
        bool mLogDebugViewToFile;
        int mArtRes;
        int mPreInitArtRes;
        int mPauseFrames;
        bool mFocusedAfterLoad;
#ifdef BEJ_BLITZ_109_7400
        bool mSkipCoinCode;
#endif
        std::string mTestBkg;
        std::string mForceBkg;
        CustomBassMusicInterface* mCustomBassMusicInterface;
        CustomSoundManager* mCustomSoundManager;
        Buffer mBadgeDumpBuffer;
        Buffer mStatsDumpBuffer;
        int mBoostCosts[NUM_BOOSTS];
        PopDRMComm* mDRMComm;
        bool mRegCodeNotNeeded;
        bool mWantDataUpdateOnFocus;
        int mWaitingForRegUpdateTicks;
        std::string mBuyCoinsURL;
        std::string mClientId;
        Image*** mShrunkenGems;
        Profile* mProfile;
        SexyString mSwitchProfileName;
        MainMenu* mMainMenu;
        Menu* mMenu;
        int mQuitCountdown;
        HighScores* mHighScores;
        Image* mCoinImage;
        int mMaxGamesPerDay;
        int64 mLastLocalTime;
        Background* mBlitzBackground;
        Background* mBlitzBackgroundLo;
        Background* mBlitzBackgroundReplay;
        Board* mBoard;
        LoaderScreen* mLoaderScreen;
        FacebookWidget* mFacebookWidget;
        TooltipManager* mTooltipManager;
        SharedRenderTarget mRestartRT;
#ifndef BEJ_BLITZ_109_7400
        BoostWidget* mBoostWidget;
#endif
        SoundInstance* mCurVoice;
        int mCurVoiceId;
        SoundInstance* mNextVoice;
        int mNextVoiceId;
        bool mInterruptCurVoice;
        SexyString mLastUser;
        bool mCreatingFBUser;
#ifdef BEJ_BLITZ_109_7400
        bool mCheckedForUpdates;
        bool mUpdaterVisible;
        bool mUpdaterShutdown;
        UpdaterWidget* mUpdater;
        SexyString mCurrentVersion;
        SexyString mPatchNoteVersion;
        std::vector<std::wstring> mPatchNotes;
#endif
        D3DObject* mTempleObject;
        std::vector<SexyString> mAffirmationFiles;
        std::vector<SexyString> mSBAFiles;
        std::vector<SexyString> mAmbientFiles;
        std::vector<SexyString> mTips;
        std::vector<SexyString> mRankNames;
#ifdef BEJ_BLITZ_109_7400
        SexyString mRankNameFormatted;
#endif
        std::vector<SexyString> mBadgeNames;
        std::vector<std::vector<int>> mBadgeCutoffs;
        std::vector<std::vector<int>> mBadgeSecondaryCutoffs;
        UnderDialogWidget* mUnderDialogWidget;
        float mDialogObscurePct;
        ButtonWidget* mWindowedButton;
        ResizeWidget* mResizeWidget;
        int mTipIdx;
        std::string mQueuedStatsCalls;
        uint mStatsGetHandle;
        bool mStatsStalled;
        std::set<HWND> mLastReturnTitledWindows;
        std::set<HWND> mCurReturnTitledWindows;

    public:
        Bej3App();
        virtual ~Bej3App();

        virtual void Init();

        void UpdateDebugViewHooks();
    };

    extern Bej3App* gApp;
}
