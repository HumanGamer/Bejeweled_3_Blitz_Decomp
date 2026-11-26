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

void MemoryImage::BitsChanged()
{
    // TODO: Implement
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

void MemoryImage::FillRect(const Rect& theRect, const Color& theColor, int theDrawMode)
{
    ulong src = theColor.ToInt();

    ulong* aBits = GetBits();

    int oldAlpha = src >> 24;

    if (oldAlpha == 0xFF)
    {
        for (int aRow = theRect.mY; aRow < theRect.mY+theRect.mHeight; aRow++)
        {
            ulong* aDestPixels = &aBits[aRow*mWidth+theRect.mX];

            for (int i = 0; i < theRect.mWidth; i++)
                *aDestPixels++ = src;
        }
    }
    else
    {
        for (int aRow = theRect.mY; aRow < theRect.mY+theRect.mHeight; aRow++)
        {
            ulong* aDestPixels = &aBits[aRow*mWidth+theRect.mX];

            for (int i = 0; i < theRect.mWidth; i++)
            {
                ulong dest = *aDestPixels;

                int aDestAlpha = dest >> 24;
                int aNewDestAlpha = aDestAlpha + ((255 - aDestAlpha) * oldAlpha) / 255;

                int newAlpha = 255 * oldAlpha / aNewDestAlpha;

                int oma = 256 - newAlpha;

#ifdef OPTIMIZE_SOFTWARE_DRAWING
                *(aDestPixels++) = (aNewDestAlpha << 24) |
                    ((((dest & 0xFF00FF) * oma + (src & 0xFF00FF) * newAlpha) >> 8) & 0xFF00FF) |
                    ((((dest & 0x00FF00) * oma + (src & 0x00FF00) * newAlpha) >> 8) & 0x00FF00);
#else
                *(aDestPixels++) = (aNewDestAlpha << 24) |
                    ((((dest & 0x0000FF) * oma) >> 8) + (((src & 0x0000FF) * newAlpha) >> 8) & 0x0000FF) |
                    ((((dest & 0x00FF00) * oma) >> 8) + (((src & 0x00FF00) * newAlpha) >> 8) & 0x00FF00) |
                    ((((dest & 0xFF0000) * oma) >> 8) + (((src & 0xFF0000) * newAlpha) >> 8) & 0xFF0000);
#endif
            }
        }
    }

    BitsChanged();
}

void MemoryImage::SetImageMode(bool hasTrans, bool hasAlpha)
{
    mForcedMode = true;
    mHasTrans = hasTrans;
    mHasAlpha = hasAlpha;
}
