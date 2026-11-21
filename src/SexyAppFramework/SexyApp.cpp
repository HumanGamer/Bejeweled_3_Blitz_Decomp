#include "SexyApp.h"

#include "Debug.h"
#include "ImageLib.h"
#include "SEHCatcher.h"

using namespace Sexy;

SexyApp* Sexy::gSexyApp = nullptr;

// TODO: Figure out how to set SIGNATURE_CODE_MARKER
// SIGNATURE_CODE_MARKER is 64578011 for 1.0.9.7400
// SIGNATURE_CODE_MARKER is 99438826 for 1.0.6.7260

// Groups of 80-byte data
const char DYNAMIC_DATA_BLOCK[400] =
        "DYN00000PACPOPPOPCAPPACPOPPOPCAPBUILDINFOMARKERPACPOPPOPCAPPACPOPPOPCAPXXXXXXXXX"
        "00000000PACPOPPOPCAPPACPOPPOPCAPBUILDINFOMARKERPACPOPPOPCAPPACPOPPOPCAPXXXXXXXXX";

const char* BUILD_INFO_MARKER           = DYNAMIC_DATA_BLOCK + 80;
const char* SIGNATURE_CODE_MARKER       = DYNAMIC_DATA_BLOCK + 80*2;
const char* BETA_ID_MARKER              = DYNAMIC_DATA_BLOCK + 80*3;

SexyApp::SexyApp()
{
    gSexyApp = this;

    mTimesPlayed = 0;
    mTimesExecuted = 0;
    mTimedOut = false;

    mCheckPartnerSig = true;
    mIsRegistered = false;
    mBuildUnlocked = false;
    mDownloadId = 0;
    mRegSource = "ingame";
    mSkipAd = false;
    mDontUpdate = false;
    mLastVerCheckQueryTime = 0;

    mDemoPrefix = "popcap";
    mDemoFileName = mDemoPrefix + ".dmo";
    mCompanyName = "PopCap";
    mFullCompanyName = "PopCap Games";

    mInternetManager = new InternetManager();
    mBetaSupport = nullptr;
    mBetaValidate = false;

    char aStr[9] = {0};
    strncpy(aStr, BUILD_INFO_MARKER, 8);
    mBuildNum = atoi(aStr);

    if (mBuildNum != 0)
        mBuildDate = BUILD_INFO_MARKER + 8;
}

SexyApp::~SexyApp()
{
    delete mBetaSupport;
    delete mInternetManager;
}

void SexyApp::Init()
{
    SexyString crashMessage =
            _S("An unexpected error has occured!  Pressing 'Send Report' ")
            _S("will send us helpful debugging information that may help ")
            _S("us resolve this issue in the future.\r\n\r\n")
            _S("You can also contact us directly at feedback@popcap.com.");

    SEHCatcher::mCrashMessage = SexyStringToWString(GetString("UNEXPECTED_ERROR", crashMessage));

    SexyString submitMessage =
            _S("Please help us out by providing as much information as ")
            _S("you can about this crash. Is this the first time it happened? ")
            _S("Have you used other PopCap Deluxe games successfully before? ")
            _S("Have you upgraded your drivers or any software recently that ")
            _S("may be interfering with this program?");

    SEHCatcher::mSubmitMessage = SexyStringToWString(GetString("PLEASE_HELP", submitMessage));

    SexyString submitErrorMessage =
            _S("Failed to connect to PopCap servers.  Please check your Internet connection.\n")
            _S("If you are on a dial-up connection, you may have to manually connect to your ISP.");

    SEHCatcher::mSubmitErrorMessage = SexyStringToWString(GetString("FAILED_CONNECT_POPCAP", submitErrorMessage));

    SEHCatcher::mSubmitHost = "www.popcap.com";

    OutputDebugStrF("Product: %s\r\n", mProdName.c_str());
    OutputDebugStrF("BuildNum: %d\r\n", mBuildNum);
    OutputDebugStrF("BuildDate: %s\r\n", mBuildDate.c_str());

    ImageLib::SetJ2KCodecKey("LU5OkT7!L06fiD9nMbCE9ZoJuXxgx4zcgVDd2!a1tb2uHfhRrLG5wnNySQswXqAt2h");

    SexyAppBase::Init();

    if (IsScreenSaver())
        mSkipAd = true;

    mTimesExecuted++;
}

void SexyApp::InitPropertiesHook()
{
    // Load properties if we need to
    bool checkSig = !IsScreenSaver();
    LoadProperties("properties\\partner.xml", false, checkSig);

    // Check to see if this build is unlocked.
    if (GetBoolean("NoReg", false))
    {
        mIsRegistered = true;
        mBuildUnlocked = true;
    }

    mProdName = ToString(GetString("ProdName", StringToSexyString(mProdName)));

    if (!mForceFullScreen)
        mIsWindowed = GetBoolean("DefaultWindowed", mIsWindowed);

    SexyString aNewTitle = GetString("Title", _S(""));
    if (!aNewTitle.empty())
        mTitle = aNewTitle + _S(" ") + StringToSexyString(mProductVersion);

    mInternetManager->Init();
    mBetaSupport = new BetaSupport(this);
}
