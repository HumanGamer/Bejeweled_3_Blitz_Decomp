#pragma once

#include "Common.h"
#include "HTTPTransfer.h"

namespace Sexy
{
    class AdInstance;

    class InternetManager
    {
    public:
        enum
        {
            VCRESULT_NONE,
            VCRESULT_CHECKING,
            VCRESULT_DONE,
            VCRESULT_FAILED,
        };

        int mNumMoreGamesAds;
        HTTPTransfer mVersionCheckTransfer;
        HTTPTransfer mFileTransfers[4];
        bool mParsed;
        bool mIsUpToDate;
        bool mDoingTransfers;
        std::string mUpdateURL;
        std::list<AdInstance> mAdInstanceList;
        std::list<AdInstance> mOldAdInstanceList;
        std::list<std::string> mPendingResourceList;

        virtual void ParseAdLine(const std::string& line, bool unknown);

        InternetManager();
        virtual ~InternetManager();
    };

    enum AdType
    {
        AdType_None,
        AdType_Popup,
        AdType_MoreGames,
    };

    class AdInstance
    {
    public:
        AdType mType;
        int mWidth;
        int mHeight;
        std::list<std::string> mResourceList;
        bool mHasAllResources;
        bool mIsNew;
        bool mRemove;
        bool mJustDownloaded;
        std::string mClickURL;
        std::string mInstanceName;

        AdInstance();
        ~AdInstance();
    };
}
