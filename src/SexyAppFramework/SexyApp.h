#pragma once

#include "Common.h"
#include "InternetManager.h"
#include "BetaSupport.h"
#include "SexyAppBase.h"

namespace Sexy
{
    class SexyApp : public SexyAppBase
    {
    public:
        InternetManager* mInternetManager;
        BetaSupport* mBetaSupport;
        std::string mBetaSupportSiteOverride;
        std::string mReferId;
        std::string mVariation;
        uint mDownloadId;
        std::string mRegSource;
        uint mLastVerCheckQueryTime;
        bool mSkipAd;
        bool mDontUpdate;
        bool mCheckPartnerSig;
        int mBuildNum;
        std::string mBuildDate;
        std::string mUserName;
        std::string mRegUserName;
        std::string mRegCode;
        bool mIsRegistered;
        bool mBuildUnlocked;
        int mTimesPlayed;
        int mTimesExecuted;
        bool mTimedOut;

    public:
        SexyApp();
        ~SexyApp();
        virtual void Init();
    };
}
