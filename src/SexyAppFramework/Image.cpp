#include "Image.h"

using namespace Sexy;

Image::Image()
{
    mWidth = 0;
    mHeight = 0;

    mNumRows = 1;
    mNumCols = 1;

    mAnimInfo = nullptr;
    mDrawn = false;
}

Image::Image(const Image &theImage) :
    mWidth(theImage.mWidth),
    mHeight(theImage.mHeight),
    mNumRows(theImage.mNumRows),
    mNumCols(theImage.mNumCols)
{
    mDrawn = false;

    // TODO: Implement
}

Image::~Image()
{
    // TODO: Implement
}

int Image::GetWidth()
{
    return mWidth;
}

int Image::GetHeight()
{
    return mHeight;
}
