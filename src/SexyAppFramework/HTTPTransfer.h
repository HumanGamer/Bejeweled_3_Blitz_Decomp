#pragma once

#include "Common.h"

namespace Sexy
{
    class HTTPTransfer
    {
    public:
        enum
        {
            RESULT_DONE,
            RESULT_NOT_STARTED,
            RESULT_NOT_COMPLETED,
            RESULT_NOT_FOUND,
            RESULT_HTTP_ERROR,
            RESULT_ABORTED,
            RESULT_SOCKET_ERROR,
            RESULT_INVALID_ADDR,
            RESULT_CONNECT_FAIL,
            RESULT_DISCONNECTED,
        };

        int mTransferId;
        int mSocket;
        std::string mSendStr;
        std::string mSpecifiedBaseURL;
        std::string mSpecifiedRelURL;
        std::string mURL;
        std::string mProto;
        std::string mHost;
        int mPort;
        std::string mPath;
        int mContentLength;
        std::string mContent;
        bool mTransferPending;
        bool mThreadRunning;
        bool mExiting;
        bool mAborted;
        int mResult;
        int mDemoLastKnownResult;

    public:
        HTTPTransfer();
        virtual ~HTTPTransfer();
    };
}
