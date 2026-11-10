#pragma once

#include "Common.h"

namespace Sexy
{
    class WinInetHTTPTransfer
    {
    public:
        enum EResult
        {
            RESULT_HTTP_REDIRECT = 5,
            RESULT_INTERNAL_ERROR = 11,
        };

        std::string mSpecifiedBaseURL;
        std::string mSpecifiedRelURL;
        std::string mURL;
        std::string mProto;
        std::string mUserName;
        std::string mUserPass;
        std::string mHost;
        int mPort;
        std::string mPath;
        std::string mAction;
        std::string mUserAgent;
        std::string mPostContentType;
        std::string mPostData;
        FILE *mFP;
        bool mUsingFile;
        std::string mContent;
        int mContentLength;
        int mCurContentLength;
        bool mTransferPending;
        bool mThreadRunning;
        bool mExiting;
        bool mAborted;
        EResult mResult;
        RTL_CRITICAL_SECTION mFileCritSection;

    public:
        WinInetHTTPTransfer();
        virtual ~WinInetHTTPTransfer();
    };
}
