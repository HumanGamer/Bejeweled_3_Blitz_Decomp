#include "DDImage.h"

#include "Debug.h"
#include "DXSurface7.h"
#include "DXSurface8.h"
#include "DXSurface9.h"
#include "IDXSurface.h"
#include "SexyAppBase.h"

using namespace Sexy;

DDImage::DDImage()
{
    mDDInterface = gSexyAppBase->mDDInterface;

    Init();
}

DDImage::DDImage(DDInterface *ddInterface)
{
    mDDInterface = ddInterface;

    Init();
}

DDImage::~DDImage()
{
    if (mSurface != nullptr)
        mSurface->Release();

    mDDInterface->RemoveDDImage(this);

	DBG_ASSERTE(mLockCount == 0);
}

void DDImage::Init()
{
    mSurface = nullptr;
    mDDInterface->AddDDImage(this);

    mNoLock = false;
    mWantDDSurface = false;
    mDrawToBits = false;
    mSurfaceSet = false;

    mLockCount = 0;
}

bool DDImage::LockSurface()
{
	if (mDDInterface->mIs3D && (mImageFlags & ImageFlag_RenderTarget) != 0)
		return false;

	if (mLockCount == 0)
	{
		memset(&mLockedSurfaceDesc, 0, sizeof(mLockedSurfaceDesc));
		mLockedSurfaceDesc.dwSize = sizeof(mLockedSurfaceDesc);
		int aResult = S_FALSE;
		if(GetSurface())
			aResult = GetSurface()->Lock(&mLockedSurfaceDesc);

		if (aResult != DD_OK)
			return false;
	}

	mLockCount++;

	DBG_ASSERTE(mLockCount < 8);

	return true;
}

bool DDImage::UnlockSurface()
{
	if (mDDInterface->mIs3D && (mImageFlags & ImageFlag_RenderTarget) != 0)
		return false;

	--mLockCount;

	if (mLockCount == 0)
	{
		mSurface->Unlock(nullptr);
	}

	DBG_ASSERTE(mLockCount >= 0);

	return true;
}

void DDImage::SetSurface(LPDIRECTDRAWSURFACE theSurface)
{
    mSurfaceSet = true;

    if (mSurface != nullptr)
    {
        int version;
        if (mDDInterface->mIsD3D8Or9)
        {
            if (mDDInterface->mIsD3D9)
                version = 9;
            else
                version = 8;
        }
        else
        {
            version = 7;
        }

        if (mSurface->GetVersion() != version)
        {
            // TODO: This might actually just be mSurface->Release(); without the check and setting to nullptr
            if (mSurface != nullptr)
                mSurface->Release();
            mSurface = nullptr;
        }
    }

    if (theSurface != nullptr)
    {
        if (mSurface == nullptr)
        {
            if (mDDInterface->mIsD3D9)
                mSurface = CreateDXSurface9(mDDInterface);
            else if (mDDInterface->mIsD3D8)
                mSurface = CreateDXSurface8(mDDInterface);
            else
                mSurface = CreateDXSurface7(mDDInterface);
        }

        mSurface->SetSurface(theSurface);
        mSurface->GetDimensions(&mWidth, &mHeight);
        mNoLock = false;
    }
    else
    {
        // TODO: This might actually just be mSurface->Release(); without the check and setting to nullptr
        if (mSurface != nullptr)
            mSurface->Release();
        mSurface = nullptr;

        mHeight = 0;
        mWidth = 0;
        mNoLock = true;
    }
}

IDXSurface *DDImage::GetSurface()
{
	//TODO: Log if generate surface fails

	if (mSurface != nullptr)
		mSurface->GenerateDDSurface(this);

	return mSurface;
}

void DDImage::BitsChanged()
{
	MemoryImage::BitsChanged();

	if (mSurface != nullptr)
		mSurface->Release();
	mSurface = nullptr;
}

void DDImage::CommitBits()
{
    if (mSurface == nullptr)
    {
        MemoryImage::CommitBits();
        return;
    }
}

ulong *DDImage::GetBits()
{
    if (mBits == nullptr)
    {
        if (mSurface == nullptr)
            return MemoryImage::GetBits();

        if (mNoLock)
            return nullptr;

        mBits = mSurface->GetBits(this);
    }

    return mBits;
}

void DDImage::FillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{
    CommitBits();
    if (mDrawToBits || mHasAlpha || mHasTrans || mDDInterface->mIs3D)
    {
        MemoryImage::FillRect(theRect, theColor, theDrawMode);
        return;
    }

    switch (theDrawMode)
    {
        case Graphics::DRAWMODE_NORMAL:
            NormalFillRect(theRect, theColor);
            break;
        case Graphics::DRAWMODE_ADDITIVE:
            AdditiveFillRect(theRect, theColor);
            break;
    }

    DeleteAllNonSurfaceData();
}

void DDImage::NormalFillRect(const Rect &theRect, const Color &theColor)
{
	if (mNoLock)
		return;

	if (!LockSurface())
		return;

	ulong aRMask = mLockedSurfaceDesc.ddpfPixelFormat.dwRBitMask;
	ulong aGMask = mLockedSurfaceDesc.ddpfPixelFormat.dwGBitMask;
	ulong aBMask = mLockedSurfaceDesc.ddpfPixelFormat.dwBBitMask;

	ulong aRRoundAdd = aRMask;
	ulong aGRoundAdd = aGMask;
	ulong aBRoundAdd = aBMask;

	if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 16)
	{
		if (theColor.mAlpha == 255)
		{
			ushort aColor =
				((((theColor.mRed * aRMask) + aRRoundAdd) >> 8) & aRMask) |
				((((theColor.mGreen * aGMask) + aGRoundAdd) >> 8) & aGMask) |
				((((theColor.mBlue * aBMask) + aBRoundAdd) >> 8) & aBMask);

			ushort* aDestPixelsRow = ((ushort*) mLockedSurfaceDesc.lpSurface) + (theRect.mY * mLockedSurfaceDesc.lPitch/2) + theRect.mX;

			for (int y = 0; y < theRect.mHeight; y++)
			{
				ushort* aDestPixels = aDestPixelsRow;

				for (int x = 0; x < theRect.mWidth; x++)
					*(aDestPixels++) = aColor;

				aDestPixelsRow += mLockedSurfaceDesc.lPitch/2;
			}
		}
		else
		{
			ushort src =
				((((((theColor.mRed * theColor.mAlpha + 0x80) >> 8) * aRMask) + aRRoundAdd) >> 8) & aRMask) +
				((((((theColor.mGreen * theColor.mAlpha + 0x80) >> 8) * aGMask) + aGRoundAdd) >> 8) & aGMask) +
				((((((theColor.mBlue * theColor.mAlpha + 0x80) >> 8) * aBMask) + aBRoundAdd) >> 8) & aBMask);
			int oma = 256 - theColor.mAlpha;

			ushort* aDestPixelsRow = ((ushort*) mLockedSurfaceDesc.lpSurface) + (theRect.mY * mLockedSurfaceDesc.lPitch/2) + theRect.mX;

			for (int y = 0; y < theRect.mHeight; y++)
			{
				ushort* aDestPixels = aDestPixelsRow;

				for (int x = 0; x < theRect.mWidth; x++)
				{
					ushort dest = *aDestPixels;

					*(aDestPixels++) = src +
						(((((dest & aRMask) * oma) + aRRoundAdd) >> 8) & aRMask) +
						(((((dest & aGMask) * oma) + aGRoundAdd) >> 8) & aGMask) +
						(((((dest & aBMask) * oma) + aBRoundAdd) >> 8) & aBMask);
				}

				aDestPixelsRow += mLockedSurfaceDesc.lPitch/2;
			}
		}
	}
	else if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		if (theColor.mAlpha == 255)
		{
			ulong aColor =
				((((theColor.mRed * aRMask)) >> 8) & aRMask) |
				((((theColor.mGreen * aGMask)) >> 8) & aGMask) |
				((((theColor.mBlue * aBMask)) >> 8) & aBMask);

			ulong* aDestPixelsRow = ((ulong*) mLockedSurfaceDesc.lpSurface) + (theRect.mY * mLockedSurfaceDesc.lPitch/4) + theRect.mX;

			for (int y = 0; y < theRect.mHeight; y++)
			{
				ulong* aDestPixels = aDestPixelsRow;

				for (int x = 0; x < theRect.mWidth; x++)
					*(aDestPixels++) = aColor;

				aDestPixelsRow += mLockedSurfaceDesc.lPitch/4;
			}
		}
		else
		{
			ulong src =
				((((((theColor.mRed * theColor.mAlpha + 0x7F) >> 8) * aRMask) + aRRoundAdd) >> 8) & aRMask) +
				((((((theColor.mGreen * theColor.mAlpha + 0x7F) >> 8) * aGMask) + aGRoundAdd) >> 8) & aGMask) +
				((((((theColor.mBlue * theColor.mAlpha + 0x7F) >> 8) * aBMask) + aBRoundAdd) >> 8) & aBMask);
			int oma = 256 - theColor.mAlpha;

			ulong* aDestPixelsRow = ((ulong*) mLockedSurfaceDesc.lpSurface) + (theRect.mY * mLockedSurfaceDesc.lPitch/4) + theRect.mX;

			for (int y = 0; y < theRect.mHeight; y++)
			{
				ulong* aDestPixels = aDestPixelsRow;

				for (int x = 0; x < theRect.mWidth; x++)
				{
					ulong dest = *aDestPixels;

					*(aDestPixels++) = src +
						(((((dest & aRMask) * oma)) >> 8) & aRMask) +
						(((((dest & aGMask) * oma)) >> 8) & aGMask) +
						(((((dest & aBMask) * oma)) >> 8) & aBMask);
				}

				aDestPixelsRow += mLockedSurfaceDesc.lPitch/4;
			}
		}
	}

	UnlockSurface();
}

void DDImage::AdditiveFillRect(const Rect &theRect, const Color &theColor)
{
	if (mNoLock)
		return;

	if (!LockSurface())
		return;

	ulong aRMask = mLockedSurfaceDesc.ddpfPixelFormat.dwRBitMask;
	ulong aGMask = mLockedSurfaceDesc.ddpfPixelFormat.dwGBitMask;
	ulong aBMask = mLockedSurfaceDesc.ddpfPixelFormat.dwBBitMask;

	ulong aRRoundAdd = aRMask >> 1;
	ulong aGRoundAdd = aGMask >> 1;
	ulong aBRoundAdd = aBMask >> 1;

	int aRedShift = mDDInterface->mRedShift;
	int aGreenShift = mDDInterface->mGreenShift;
	int aBlueShift = mDDInterface->mBlueShift;

	int* aMaxRedTable = mDDInterface->mRedAddTable;
	int* aMaxGreenTable = mDDInterface->mGreenAddTable;
	int* aMaxBlueTable = mDDInterface->mBlueAddTable;

	if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 16)
	{
		ushort rc = ((theColor.mRed * theColor.mAlpha) / 255) >> (8-mDDInterface->mRedBits);
		ushort gc = ((theColor.mGreen * theColor.mAlpha) / 255) >> (8-mDDInterface->mGreenBits);
		ushort bc = ((theColor.mBlue * theColor.mAlpha) / 255) >> (8-mDDInterface->mBlueBits);

		ushort* aDestPixelsRow = ((ushort*) mLockedSurfaceDesc.lpSurface) + (theRect.mY * mLockedSurfaceDesc.lPitch/2) + theRect.mX;

		for (int y = 0; y < theRect.mHeight; y++)
		{
			ushort* aDestPixels = aDestPixelsRow;

			for (int x = 0; x < theRect.mWidth; x++)
			{
				ushort dest = *aDestPixels;

				int r = aMaxRedTable[((dest & aRMask) >> aRedShift) + rc];
				int g = aMaxGreenTable[((dest & aGMask) >> aGreenShift) + gc];
				int b = aMaxBlueTable[((dest & aBMask) >> aBlueShift) + bc];

				*(aDestPixels++) =
					(r << aRedShift) |
					(g << aGreenShift) |
					(b << aBlueShift);
			}

			aDestPixelsRow += mLockedSurfaceDesc.lPitch/2;
		}
	}
	else if (mLockedSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		ulong rc = ((theColor.mRed * theColor.mAlpha) / 255) >> (8-mDDInterface->mRedBits);
		ulong gc = ((theColor.mGreen * theColor.mAlpha) / 255) >> (8-mDDInterface->mGreenBits);
		ulong bc = ((theColor.mBlue * theColor.mAlpha) / 255) >> (8-mDDInterface->mBlueBits);

		ulong* aDestPixelsRow = ((ulong*) mLockedSurfaceDesc.lpSurface) + (theRect.mY * mLockedSurfaceDesc.lPitch/4) + theRect.mX;

		for (int y = 0; y < theRect.mHeight; y++)
		{
			ulong* aDestPixels = aDestPixelsRow;

			for (int x = 0; x < theRect.mWidth; x++)
			{
				ulong dest = *aDestPixels;

				int r = aMaxRedTable[((dest & aRMask) >> aRedShift) + rc];
				int g = aMaxGreenTable[((dest & aGMask) >> aGreenShift) + gc];
				int b = aMaxBlueTable[((dest & aBMask) >> aBlueShift) + bc];

				*(aDestPixels++) =
					(r << aRedShift) |
					(g << aGreenShift) |
					(b << aBlueShift);
			}

			aDestPixelsRow += mLockedSurfaceDesc.lPitch/4;
		}
	}

	UnlockSurface();
}

void DDImage::DeleteAllNonSurfaceData()
{
    delete[] mBits;
    mBits = nullptr;

    delete[] mNativeAlphaData;
    mNativeAlphaData = nullptr;

    delete[] mRLAdditiveData;
    mRLAdditiveData = nullptr;

    delete[] mRLAlphaData;
    mRLAlphaData = nullptr;

    delete[] mColorTable;
    mColorTable = nullptr;

    delete[] mColorIndices;
    mColorIndices = nullptr;
}
