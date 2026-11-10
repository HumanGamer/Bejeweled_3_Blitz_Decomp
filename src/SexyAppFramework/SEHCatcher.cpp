#include "SEHCatcher.h"
#include "SexyAppBase.h"
#include <fstream>
#include <process.h>

using namespace Sexy;

LPTOP_LEVEL_EXCEPTION_FILTER SEHCatcher::mPreviousFilter;
SexyAppBase*				SEHCatcher::mApp = nullptr;
HFONT						SEHCatcher::mDialogFont = nullptr;
HFONT						SEHCatcher::mBoldFont = nullptr;
bool						SEHCatcher::mHasDemoFile = false;
bool						SEHCatcher::mDebugError = false;
std::string					SEHCatcher::mErrorTitle;
std::string					SEHCatcher::mErrorText;
std::string					SEHCatcher::mUserText;
std::string					SEHCatcher::mUploadFileName;
HWND						SEHCatcher::mYesButtonWindow = nullptr;
HWND						SEHCatcher::mNoButtonWindow = nullptr;
HWND						SEHCatcher::mDebugButtonWindow = nullptr;
HWND						SEHCatcher::mEditWindow = nullptr;
HMODULE						SEHCatcher::mKernelLib = nullptr;
HMODULE						SEHCatcher::mImageHelpLib = nullptr;
SYMINITIALIZEPROC			SEHCatcher::mSymInitialize = nullptr;
SYMSETOPTIONSPROC			SEHCatcher::mSymSetOptions = nullptr;
UNDECORATESYMBOLNAMEPROC	SEHCatcher::mUnDecorateSymbolName = nullptr;
SYMCLEANUPPROC				SEHCatcher::mSymCleanup = nullptr;
STACKWALKPROC				SEHCatcher::mStackWalk = nullptr;
SYMFUNCTIONTABLEACCESSPROC	SEHCatcher::mSymFunctionTableAccess = nullptr;
SYMGETMODULEBASEPROC		SEHCatcher::mSymGetModuleBase = nullptr;
SYMGETSYMFROMADDRPROC		SEHCatcher::mSymGetSymFromAddr = nullptr;
HTTPTransfer				SEHCatcher::mSubmitReportTransfer;
bool						SEHCatcher::mExiting = false;
bool						SEHCatcher::mShowUI = true;
bool						SEHCatcher::mAllowSubmit = true;
bool						SEHCatcher::mAllowMinidumps = false;
bool						SEHCatcher::mAllowUltradumps = false;

std::wstring					SEHCatcher::mCrashMessage = L"An unexpected error has occured!";
std::string						SEHCatcher::mSubmitHost;
std::wstring					SEHCatcher::mSubmitMessage = L"Please help out by providing as much information as you can about this crash.";
std::wstring					SEHCatcher::mSubmitErrorMessage = L"Failed to connect to server.";

static bool gUseDefaultFonts = true;

SEHCatcher::SEHCatcher()
{
    mPreviousFilter = nullptr;
    this->Init();
}

SEHCatcher::~SEHCatcher()
{
    SetUnhandledExceptionFilter(mPreviousFilter);
}

void SEHCatcher::Init()
{
    if (mPreviousFilter == nullptr)
        mPreviousFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
}

bool SEHCatcher::HandleCmdLineParam(const std::string &theParamName, const std::string &theParamValue)
{
    if (theParamName == "-minidump")
    {
        mAllowMinidumps = true;
        return true;
    }

    if (theParamName == "-ultradump")
    {
        mAllowUltradumps = true;
        return true;
    }

    return false;
}

long __stdcall SEHCatcher::UnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptPtr)
{
    if (mApp != NULL)
        mApp->SEHOccured();

    DoHandleDebugEvent(lpExceptPtr);

    if (!mDebugError)
        SetErrorMode(SEM_NOGPFAULTERRORBOX);

    return EXCEPTION_CONTINUE_SEARCH;
}

void SEHCatcher::DoHandleDebugEvent(LPEXCEPTION_POINTERS lpEP)
{
    // TODO: Implement
}
