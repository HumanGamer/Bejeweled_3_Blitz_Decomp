#include "PopDRMComm.h"

const ulong PopDRMComm::Web::invalid_handle = -1;

PopDRMComm::PopDRMComm()
{
    mThread = nullptr;
    mThreadId = 0;
    mEvent = nullptr;
    mHwnd = nullptr;
    mDRMHwnd = nullptr;
    mFileHandle = nullptr;
    mDataPtr = nullptr;

    mRequiredThread = GetCurrentThreadId();

    mLockMsg = RegisterWindowMessageA("PopCapDRM_EnableLocking");
    mTimingMsg = RegisterWindowMessageA("PopCapDRM_EnableTiming");
    mQueryMsg = RegisterWindowMessageA("PopCapDRM_QueryData");
    mConnectMsg = RegisterWindowMessageA("PopCapDrm_IPC");
    mResponseMsg = RegisterWindowMessageA("PopCapDrm_IPC_Response");
    mForceLockMsg = RegisterWindowMessageA("PopCapDrm_ForceLock");
    mBuyGameMsg = RegisterWindowMessageA("PopCapDrm_BuyGame");
    mShowScreenMsg = RegisterWindowMessageA("PopCapDRM_ShowScreen");
    mWaitMsg = RegisterWindowMessageA("PopCapDRM_WaitUI");

    mConnected = false;

    InitializeCriticalSection(&mStringCritSection);
    InitWindow();

    mWeb = new Web(this);
}

PopDRMComm::~PopDRMComm()
{
    if (mThread != nullptr)
    {
        PostThreadMessage(mThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(mThread, INFINITE);
        CloseHandle(mThread);
    }

    if (mDataPtr != nullptr)
        UnmapViewOfFile(mDataPtr);

    if (mFileHandle != nullptr)
        CloseHandle(mFileHandle);

    DeleteCriticalSection(&mStringCritSection);

    if (mWeb != nullptr)
        delete mWeb;
}

void PopDRMComm::InitWindow()
{
    mEvent = CreateEvent(nullptr, false, false, nullptr);
    mThread = CreateThread(nullptr, 0, ThreadProc, this, 0, &mThreadId);
    WaitForSingleObject(mEvent, INFINITE);
    CloseHandle(mEvent);
    mEvent = nullptr;
}

void PopDRMComm::InitFileMapping()
{
    if (mFileHandle == nullptr)
    {
        char buffer[16];
        sprintf(buffer, "HWND%08.8X", mDRMHwnd);

        mFileHandle = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 0x10000, buffer);

        if (mFileHandle != nullptr)
        {
            mDataPtr = MapViewOfFile(mFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        }
    }
}

void PopDRMComm::BuyGame(const char *str)
{
    if (!IsWindow(mDRMHwnd))
    {
        mConnected = false;
        return;
    }

    InitFileMapping();
    EnterCriticalSection(&mStringCritSection);

    *(DWORD*)mDataPtr = strlen(str);
    memcpy((char*)mDataPtr + 4, str, strlen(str));

    SendMessage(mDRMHwnd, mBuyGameMsg, 0, 1);

    LeaveCriticalSection(&mStringCritSection);
}

DWORD CALLBACK PopDRMComm::ThreadProc(LPVOID lpThreadParameter)
{
    PopDRMComm* self = (PopDRMComm*)lpThreadParameter;
    WNDCLASSA wc;
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = "PopDRMComm_Game";
    wc.hInstance = GetModuleHandle(nullptr);
    RegisterClassA(&wc);

    self->mHwnd = CreateWindowExA(0, "PopDRMComm_Game", nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (IsWindow(self->mHwnd))
    {
        SetWindowLong(self->mHwnd, GWL_USERDATA, (LONG) self);
        SetEvent(self->mEvent);

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0) != 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DWORD result = IsWindow(self->mHwnd);
    if (result != 0)
        return DestroyWindow(self->mHwnd);

    return result;
}

LRESULT __stdcall PopDRMComm::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PopDRMComm* self = (PopDRMComm*) GetWindowLong(hWnd, GWL_USERDATA);

    if (self != nullptr && uMsg == self->mResponseMsg && wParam == GetCurrentProcessId())
    {
        self->mDRMHwnd = (HWND) lParam;
        self->mConnected = true;
        if (self->mEvent != nullptr)
            SetEvent(self->mEvent);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

PopDRMComm::Web::Web(PopDRMComm *comm)
{
    mComm = comm;
    mBeginGetMsg = RegisterWindowMessageA("PopCapDRM_Web_GetTransferStatus");
    mFinishGetMsg = RegisterWindowMessageA("PopCapDRM_Web_FinishGet");
}

PopDRMComm::Web::~Web()
{
    // TODO: Implement (this wasn't in the binary, but was there in the pdb, likely inlined somewhere and needs to be found)
}

ulong PopDRMComm::Web::BeginGet(const std::string &URL)
{
    if (!IsWindow(mComm->mDRMHwnd))
        return invalid_handle;

    mComm->InitFileMapping();

    EnterCriticalSection(&mComm->mStringCritSection);

    // TODO: This is really ugly but appears to be correct?
    *(DWORD*)mComm->mDataPtr = URL.length();
    memcpy((char*)mComm->mDataPtr + 4, URL.c_str(), URL.length());

    return SendMessage(mComm->mDRMHwnd, mBeginGetMsg, 0, 0);
}

PopDRMComm::Web::EDownloaderStatus PopDRMComm::Web::GetTransferStatus(const ulong handle)
{
    // TODO: Implement (this wasn't in the binary, but was there in the pdb, likely inlined somewhere and needs to be found)
    return STATUS_Done;
}
