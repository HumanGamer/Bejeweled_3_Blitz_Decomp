#include "SexyApp.h"

#include "ImageLib.h"

Sexy::SexyApp::SexyApp()
{
    // TODO: Implement
}

Sexy::SexyApp::~SexyApp()
{
    // TODO: Implement
}

void Sexy::SexyApp::Init()
{
    // TODO: Implement

    ImageLib::SetJ2KCodecKey("LU5OkT7!L06fiD9nMbCE9ZoJuXxgx4zcgVDd2!a1tb2uHfhRrLG5wnNySQswXqAt2h");

    SexyAppBase::Init();

    mTimesExecuted++;

    if (IsScreenSaver())
        mSkipAd = true;
}