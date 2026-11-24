#include "D3DInterface.h"

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

void D3DInterface::Cleanup()
{
    // TODO: Implement
}

bool D3DInterface::SceneBegun()
{
    // TODO: Implement
    return false;
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
