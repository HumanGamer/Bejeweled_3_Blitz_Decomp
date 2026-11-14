#pragma once

#include <Common.h>

class PopDRMComm
{
public:
    class Web
    {
    public:
        enum EDownloaderStatus
        {
            STATUS_Done,
            STATUS_NotStarted,
            STATUS_Pending,
            STATUS_FileNotFound,
            STATUS_ServerError,
            STATUS_Aborted,
            STATUS_SocketError,
            STATUS_InvalidAddress,
            STATUS_ConnectFailed,
            STATUS_Disconnected,
            STATUS_InternalError,
        };

        static const ulong invalid_handle;

        PopDRMComm* mComm;
        DWORD mBeginGetMsg;
        DWORD mGetTransferStatusMsg;
        DWORD mFinishGetMsg;

        Web(PopDRMComm* comm);
        ~Web();

        ulong BeginGet(const std::string& URL);
        EDownloaderStatus GetTransferStatus(const ulong handle);
    };

    HWND mHwnd;
    HWND mDRMHwnd;
    HANDLE mFileHandle;
    void* mDataPtr;
    bool mConnected;
    HANDLE mEvent;
    HANDLE mThread;
    DWORD mThreadId;
    DWORD mRequiredThread;
    int mLockMsg;
    int mTimingMsg;
    int mQueryMsg;
    int mConnectMsg;
    int mResponseMsg;
    int mForceLockMsg;
    int mBuyGameMsg;
    int mShowScreenMsg;
    int mWaitMsg;
    CRITICAL_SECTION mStringCritSection;
    Web* mWeb; // for some reason this was missing in the PDB

public:
    PopDRMComm();
    ~PopDRMComm();

    void InitWindow();
    void InitFileMapping();
    void BuyGame(const char* str = "_ingame");

    Web* GetWeb() const { return mWeb; }

    static DWORD CALLBACK ThreadProc(LPVOID lpThreadParameter);
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
