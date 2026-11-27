#include "D3DInterface.h"

#include "Graphics.h"

using namespace Sexy;

std::string D3DInterface::sErrorString;

bool gD3DInterfacePreDrawError = false;

D3DInterface::D3DInterface()
{
    // TODO: Implement
}

D3DInterface::~D3DInterface()
{
    // TODO: Implement
}

HRenderContext D3DInterface::CreateContext(Image *theImage, const HRenderContext &theContext)
{
    // TODO: Implement
    return {0};
}

void D3DInterface::DeleteContext(const HRenderContext &context)
{
    // TODO: Implement
}

void D3DInterface::SetCurrentContext(const HRenderContext &context)
{
    // TODO: Implement
}

void D3DInterface::FillRect(const Rect &theRect, const Color &theColor, int theDrawMode)
{
    // TODO: Implement
}

void D3DInterface::Cleanup()
{
    // TODO: Implement
}

bool D3DInterface::SceneBegun()
{
    // TODO: Implement
    return false;
}

void D3DInterface::RemoveImageRenderData(MemoryImage *theImage)
{
    // TODO: Implement
}

IUnknown *D3DInterface::GetBackBufferSurface()
{
    // TODO: Implement
    return nullptr;
}

bool D3DInterface::SetRenderTargetSurface(IUnknown *surface)
{
    // TODO: Implement
    return false;
}

bool D3DInterface::RecoverImageBitsFromRenderData(MemoryImage *theImage)
{
    // TODO: Implement
    return false;
}
