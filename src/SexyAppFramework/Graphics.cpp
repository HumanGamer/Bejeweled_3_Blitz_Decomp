#include "Graphics.h"

#include "Color.h"

using namespace Sexy;

Graphics::Graphics(const Graphics &theGraphics)
{
    // TODO: Implement
}

Graphics::Graphics(Image *theDestImage)
{
    // TODO: Implement
}

Graphics::~Graphics()
{
    // TODO: Implement
}

void Graphics::SetFont(Font *theFont)
{
    // TODO: Implement
}

Font *Graphics::GetFont()
{
    // TODO: Implement
    return nullptr;
}

void Graphics::SetColor(const Color &theColor)
{
    // TODO: Implement
}

const Color &Graphics::GetColor()
{
    // TODO: Implement
    return {};
}

void Graphics::FillRect(int theX, int theY, int theWidth, int theHeight)
{
    // TODO: Implement
}

void Graphics::FillRect(const Rect &theRect)
{
    // TODO: Implement
}

void Graphics::DrawString(const SexyString &theString, int theX, int theY)
{
    // TODO: Implement
}
