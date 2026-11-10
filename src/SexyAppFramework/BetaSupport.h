#pragma once

#include "Buffer.h"
#include "Common.h"
#include "WinInetHTTPTransfer.h"

namespace Sexy
{
    class SexyApp;

    class BetaSupport
    {
    public:
        class DataLogEntry
        {
            Buffer mBuffer;
            std::string mFileName;
            std::string mFileExt;
        };

        SexyApp* mApp;
        HFONT mDialogFont;
        HFONT mItalicsFont;
        HFONT mBoldFont;
        int mPeriodCount;
        HWND mHWnd;
        HWND mTextHWnd;
        HWND mNameHWnd;
        HWND mPasswordHWnd;
        WinInetHTTPTransfer mWinInetHTTPTransfer;
        bool mAborted;
        bool mDialogClosed;
        std::string mBetaCommentURL;
        std::string mDemoUploadFile;
        std::string mName;
        std::string mPassword;
        bool mValidated;
        std::map<std::string, BetaSupport::DataLogEntry> mDataLogs;

    public:
        BetaSupport();
        virtual ~BetaSupport();
    };
}
