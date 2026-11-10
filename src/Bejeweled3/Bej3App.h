#pragma once

#include <Common.h>

#include "SexyApp.h"

namespace Sexy
{
    class Bej3App : public SexyApp
    {
    public:
        double mVoiceVolume;
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
        std::string mTestBkg;
        std::string mForceBkg;
        // TODO: Implement more fields

    public:
        Bej3App();
        virtual ~Bej3App();

        virtual void Init();
    };

    extern Bej3App* gApp;
}
