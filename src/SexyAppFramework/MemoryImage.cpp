#include "MemoryImage.h"

#include "SexyAppBase.h"

using namespace Sexy;

#ifdef OPTIMIZE_SOFTWARE_DRAWING
bool gOptimizeSoftwareDrawing = false;
#endif

MemoryImage::MemoryImage()
{
    mApp = gSexyAppBase;

    Init();
}

MemoryImage::MemoryImage(SexyAppBase *theApp)
{
    mApp = theApp;

    Init();
}

MemoryImage::MemoryImage(const MemoryImage &theMemoryImage)
{
    // TODO: Implement
}

MemoryImage::~MemoryImage()
{
    mApp->RemoveMemoryImage(this);

    delete [] mBits;
    delete [] mNativeAlphaData;
    delete [] mRLAlphaData;
    delete [] mRLAdditiveData;
    delete [] mColorIndices;
    delete [] mColorTable;
}

void MemoryImage::Init()
{
    mBits = nullptr;
    mColorTable = nullptr;
    mColorIndices = nullptr;

    mNativeAlphaData = nullptr;
    mRLAlphaData = nullptr;
    mRLAdditiveData = nullptr;
    mHasTrans = false;
    mHasAlpha = false;
    mBitsChanged = false;
    mForcedMode = false;
    mIsVolatile = false;

    mBitsChangedCount = 0;

    mPurgeBits = false;
    mWantPal = false;
    mDither16 = false;

    mApp->AddMemoryImage(this);
}

void MemoryImage::CommitBits()
{
    // TODO: Implement
}

ulong *MemoryImage::GetBits()
{
    // TODO: Implement
    return nullptr;
}

void MemoryImage::SetImageMode(bool hasTrans, bool hasAlpha)
{
    mForcedMode = true;
    mHasTrans = hasTrans;
    mHasAlpha = hasAlpha;
}
