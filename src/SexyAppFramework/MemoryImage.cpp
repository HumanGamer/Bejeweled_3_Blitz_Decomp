#include "MemoryImage.h"

#include "Debug.h"
#include "Graphics.h"
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

MemoryImage::MemoryImage(const MemoryImage &theMemoryImage) :
    Image(theMemoryImage),
    mApp(theMemoryImage.mApp),
    mHasAlpha(theMemoryImage.mHasAlpha),
    mHasTrans(theMemoryImage.mHasTrans),
    mBitsChanged(theMemoryImage.mBitsChanged),
    mIsVolatile(theMemoryImage.mIsVolatile),
    mPurgeBits(theMemoryImage.mPurgeBits),
    mWantPal(theMemoryImage.mWantPal),
    mBitsChangedCount(theMemoryImage.mBitsChangedCount)
{
    bool deleteBits = false;

    MemoryImage* aNonConstMemoryImage = (MemoryImage*) &theMemoryImage;

    if (theMemoryImage.mBits == nullptr && theMemoryImage.mColorTable == nullptr)
    {
        // Must be a DDImage with only a DDSurface
        aNonConstMemoryImage->GetBits();
        deleteBits = true;
    }

    if (theMemoryImage.mBits != nullptr)
    {
        mBits = new ulong[mWidth * mHeight + 1];
        mBits[mWidth * mHeight] = MEMORYCHECK_ID;
        memcpy(mBits, theMemoryImage.mBits, (mWidth * mHeight + 1)*sizeof(ulong));
    }
    else
        mBits = nullptr;

    if (deleteBits)
    {
        // Remove the temporary source bits
        delete [] aNonConstMemoryImage->mBits;
        aNonConstMemoryImage->mBits = nullptr;
    }

    if (theMemoryImage.mColorTable != nullptr)
    {
        mColorTable = new ulong[256];
        memcpy(mColorTable, theMemoryImage.mColorTable, 256 * sizeof(ulong));
    }
    else
        mColorTable = nullptr;

    if (theMemoryImage.mColorIndices != nullptr)
    {
        mColorIndices = new uchar[mWidth * mHeight];
        memcpy(mColorIndices, theMemoryImage.mColorIndices, mWidth * mHeight * sizeof(uchar));
    }
    else
        mColorIndices = nullptr;

    if (theMemoryImage.mNativeAlphaData != nullptr)
    {
        if (theMemoryImage.mColorTable == nullptr)
        {
            mNativeAlphaData = new ulong[mWidth * mHeight];
            memcpy(mNativeAlphaData, theMemoryImage.mNativeAlphaData, mWidth * mHeight * sizeof(ulong));
        }
        else
        {
            mNativeAlphaData = new ulong[256];
            memcpy(mNativeAlphaData, theMemoryImage.mNativeAlphaData, 256 * sizeof(ulong));
        }
    }
    else
        mNativeAlphaData = nullptr;

    if (theMemoryImage.mRLAlphaData != nullptr)
    {
        mRLAlphaData = new uchar[mWidth * mHeight];
        memcpy(mRLAlphaData, theMemoryImage.mRLAlphaData, mWidth * mHeight);
    }
    else
        mRLAlphaData = nullptr;

    if (theMemoryImage.mRLAdditiveData != nullptr)
    {
        mRLAdditiveData = new uchar[mWidth * mHeight];
        memcpy(mRLAdditiveData, theMemoryImage.mRLAdditiveData, mWidth * mHeight);
    }
    else
        mRLAdditiveData = nullptr;

    mApp->AddMemoryImage(this);
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

MemoryImage *MemoryImage::AsMemoryImage()
{
    return this;
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

void MemoryImage::PurgeBits()
{
    // TODO: Implement
}

void MemoryImage::ReInit()
{
    // Fix any un-palletizing
    if (mWantPal)
        Palletize();

    if (mPurgeBits)
        PurgeBits();
}

void MemoryImage::BitsChanged()
{
    mBitsChanged = true;
    mBitsChangedCount++;

    delete [] mNativeAlphaData;
    mNativeAlphaData = nullptr;

    delete [] mRLAlphaData;
    mRLAlphaData = nullptr;

    delete [] mRLAdditiveData;
    mRLAdditiveData = nullptr;

    mNormalTriRep.mLevels.clear();
    mAdditiveTriRep.mLevels.clear();

    // Verify secret value at end to protect against overwrite
    if (mBits != nullptr)
    {
        DBG_ASSERTE(mBits[mWidth*mHeight] == MEMORYCHECK_ID);
    }
}

void MemoryImage::CommitBits()
{
    //if (gDebug)
    //	mApp->CopyToClipboard("+MemoryImage::CommitBits");

    if (mBitsChanged)
    {
        if (!mForcedMode)
        {
            // Analyze
            if (mBits != nullptr)
            {
                mHasTrans = false;
                mHasAlpha = false;

                int aSize = mWidth*mHeight;
                ulong* ptr = mBits;

                for (int i = 0; i < aSize; i++)
                {
                    uchar anAlpha = (uchar) (*ptr++ >> 24);

                    if (anAlpha == 0)
                        mHasTrans = true;
                    else if (anAlpha != 255)
                        mHasAlpha = true;
                }
            }
            else if (mColorTable != nullptr)
            {
                mHasTrans = false;
                mHasAlpha = false;

                int aSize = 256;
                ulong* ptr = mColorTable;

                for (int i = 0; i < aSize; i++)
                {
                    uchar anAlpha = (uchar) (*ptr++ >> 24);

                    if (anAlpha == 0)
                        mHasTrans = true;
                    else if (anAlpha != 255)
                        mHasAlpha = true;
                }
            }
            else
            {
                mHasTrans = true;
                mHasAlpha = false;
            }

        }
        if (!mIsVolatile)
            BuildTriReps(true);
        mBitsChanged = false;
    }

    //if (gDebug)
    //	mApp->CopyToClipboard("-MemoryImage::CommitBits");
}

HRenderContext MemoryImage::CreateContext(Image *theImage, const HRenderContext &theContext) { return {1}; }

void MemoryImage::DeleteContext(const HRenderContext &context) {}
void MemoryImage::SetCurrentContext(const HRenderContext &context) {}

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

ulong *MemoryImage::GetBits()
{
    if (mBits == nullptr)
    {
        int aSize = mWidth * mHeight;

        mBits = new ulong[aSize + 1];
        mBits[aSize] = MEMORYCHECK_ID;

        if (mColorTable != nullptr)
        {
            for (int i = 0; i < aSize; i++)
                mBits[i] = mColorTable[mColorIndices[i]];

            delete [] mColorIndices;
            mColorIndices = nullptr;

            delete [] mColorTable;
            mColorTable = nullptr;

            delete [] mNativeAlphaData;
            mNativeAlphaData = nullptr;
        }
        else if (mNativeAlphaData != nullptr)
        {
            NativeDisplay* aDisplay = gSexyAppBase->mDDInterface;

            const int rMask = aDisplay->mRedMask;
            const int gMask = aDisplay->mGreenMask;
            const int bMask = aDisplay->mBlueMask;

            const int rLeftShift = aDisplay->mRedShift + (aDisplay->mRedBits);
            const int gLeftShift = aDisplay->mGreenShift + (aDisplay->mGreenBits);
            const int bLeftShift = aDisplay->mBlueShift + (aDisplay->mBlueBits);

            ulong* aDestPtr = mBits;
            ulong* aSrcPtr = mNativeAlphaData;

            int aSize = mWidth * mHeight;
            for (int i = 0; i < aSize; i++)
            {
                ulong val = *(aSrcPtr++);

                int anAlpha = val >> 24;

                ulong r = (((((val & rMask) << 8) / (anAlpha+1)) & rMask) << 8) >> rLeftShift;
                ulong g = (((((val & gMask) << 8) / (anAlpha+1)) & gMask) << 8) >> gLeftShift;
                ulong b = (((((val & bMask) << 8) / (anAlpha+1)) & bMask) << 8) >> bLeftShift;

                *(aDestPtr++) = (r << 16) | (g << 8) | (b) | (anAlpha << 24);
            }
        }
        else if (mRenderData == nullptr || !mApp->mDDInterface->mRenderDevice3D->RecoverImageBitsFromRenderData(this))
        {
            ZeroMemory(mBits, aSize*sizeof(ulong));
        }
    }

    return mBits;
}

void MemoryImage::SetImageMode(bool hasTrans, bool hasAlpha)
{
    mForcedMode = true;
    mHasTrans = hasTrans;
    mHasAlpha = hasAlpha;
}

bool MemoryImage::BuildTriRep(TriRep *rep, bool additive, bool force)
{
    // TODO: Implement
    return false;
}

bool MemoryImage::BuildTriReps(bool force)
{
    bool normal = (!this->mHasAlpha && !this->mHasTrans) || this->BuildTriRep(&this->mNormalTriRep, false, force);
    bool additive = this->BuildTriRep(&this->mAdditiveTriRep, true, force);
    return normal && additive;
}

bool MemoryImage::Palletize()
{
    // TODO: Implement
    return false;
}
