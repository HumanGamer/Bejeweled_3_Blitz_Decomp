#include "Graphics.h"

#include "Color.h"
#include "Font.h"
#include "Image.h"
#include "MemoryImage.h"
#include "SexyAppBase.h"

using namespace Sexy;

Image GraphicsState::mStaticImage;

//////////////////////////////////////////////////////////////////////////

void GraphicsState::CopyStateFrom(const GraphicsState* theState)
{
    mDestImage = theState->mDestImage;
    mTransX = theState->mTransX;
    mTransY = theState->mTransY;
    mClipRect = theState->mClipRect;
    mFont = theState->mFont;
    mPushedColorVector = theState->mPushedColorVector;
    mColor = theState->mColor;
    mFinalColor = theState->mFinalColor;
    mDrawMode = theState->mDrawMode;
    mColorizeImages = theState->mColorizeImages;
    mFastStretch = theState->mFastStretch;
    mWriteColoredString = theState->mWriteColoredString;
    mLinearBlend = theState->mLinearBlend;
    mScaleX = theState->mScaleX;
    mScaleY = theState->mScaleY;
    mScaleOrigX = theState->mScaleOrigX;
    mScaleOrigY = theState->mScaleOrigY;
    mIs3D = theState->mIs3D;
}

//////////////////////////////////////////////////////////////////////////

Graphics::Graphics(const Graphics &theGraphics)
{
    CopyStateFrom(&theGraphics);
    InitRenderInfo(&theGraphics);
}

Graphics::Graphics(Image *theDestImage)
{
    mTransX = 0;
    mTransY = 0;
    mScaleX = 1;
    mScaleY = 1;
    mScaleOrigX = 0;
    mScaleOrigY = 0;
    mFont = nullptr;
    mDestImage = theDestImage;
    mDrawMode = DRAWMODE_NORMAL;
    mColorizeImages = false;
    mFastStretch = false;
    mWriteColoredString = true;
    mLinearBlend = false;

    if (theDestImage == nullptr)
        mDestImage = &mStaticImage;

    mClipRect = Rect(0, 0, mDestImage->GetWidth(), mDestImage->GetHeight());

    InitRenderInfo(nullptr);
}

Graphics::~Graphics()
{
    mRenderDevice->DeleteContext(mRenderContext);
    mStateStack.clear();
}

void Graphics::InitRenderInfo(const Graphics *theGraphics)
{
    static NullRenderDevice nullRenderDevice;

    mRenderContext = {}; // TODO: Not in original but can't seem to find where this is set to 0/null?
    mGraphics3D = nullptr;
    mIs3D = false;

    RenderDevice3D* renderDevice3D = gSexyAppBase->mDDInterface->mRenderDevice3D;
    if (renderDevice3D != nullptr)
    {
        HRenderContext c;
        if (theGraphics != nullptr)
        {
            c = renderDevice3D->CreateContext(mDestImage, theGraphics->mRenderContext);
        }
        else
        {
            HRenderContext emptyContext{};
            c = renderDevice3D->CreateContext(mDestImage, emptyContext);
        }

        if (c.mHandleDword != 0)
        {
            mRenderDevice = renderDevice3D;
            mRenderContext = c;
            mGraphics3D = new Graphics3D(this, renderDevice3D, mRenderContext);
            mIs3D = true;
        }
    }

    if (mRenderContext.mHandleDword == 0)
    {
        MemoryImage* mem;
        if (mDestImage != nullptr && (mem = mDestImage->AsMemoryImage()) != nullptr)
            mRenderDevice = mem;
        else
            mRenderDevice = &nullRenderDevice;

        HRenderContext emptyContext{};
        mRenderContext = mRenderDevice->CreateContext(mDestImage, emptyContext);

        if (mGraphics3D != nullptr)
        {
            delete mGraphics3D;
            mGraphics3D = nullptr;
        }

        mIs3D = false;
    }
}

void Graphics::CalcFinalColor()
{
    if (mPushedColorVector.empty())
    {
        mFinalColor = mColor;
        return;
    }

    Color aColor = mPushedColorVector.back();

    int alpha = aColor.mAlpha * mColor.mAlpha / 255;
    if (alpha > 255)
        alpha = 255;

    int blue = aColor.mBlue * mColor.mBlue / 255;
    if (blue > 255)
        blue = 255;

    int green = aColor.mGreen * mColor.mGreen / 255;
    if (green > 255)
        green = 255;

    int red = aColor.mRed * mColor.mRed / 255;
    if (red > 255)
        red = 255;

    mFinalColor = Color(red, green, blue, alpha);
}

void Graphics::SetAsCurrentContext()
{
    mRenderDevice->SetCurrentContext(mRenderContext);
}

void Graphics::SetFont(Font *theFont)
{
    mFont = theFont;
}

Font *Graphics::GetFont()
{
    return mFont;
}

void Graphics::SetColor(const Color &theColor)
{
	mColor = theColor;

    CalcFinalColor();
}

const Color &Graphics::GetColor()
{
    return mColor;
}

void Graphics::FillRect(int theX, int theY, int theWidth, int theHeight)
{
    const Color& aColor = (!mPushedColorVector.empty()) ? mFinalColor : mColor;
    if (aColor.mAlpha == 0)
        return;

    SetAsCurrentContext();

    Rect aDestRect(theX + mTransX, theY + mTransY, theWidth, theHeight);
    mRenderDevice->FillRect(mClipRect.Intersection(aDestRect), aColor, mDrawMode);
}

void Graphics::FillRect(const Rect &theRect)
{
    FillRect(theRect.mX, theRect.mY, theRect.mWidth, theRect.mHeight);
}

void Graphics::DrawString(const SexyString &theString, int theX, int theY)
{
    if (mFont == nullptr)
        return;

    const Color& aColor = (!mPushedColorVector.empty()) ? mFinalColor : mColor;

    mFont->DrawString(this, theX, theY, theString, aColor, mClipRect);
}

Graphics3D::Graphics3D(Graphics *theGraphics, RenderDevice3D *theRenderDevice, const HRenderContext &theContext)
{
    mGraphics = theGraphics;
    mRenderDevice = theRenderDevice;
    mRenderContext = theContext;
}
