#include "D3D8Interface.h"

using namespace Sexy;

D3D8Interface::D3D8Interface()
{
    // TODO: Implement
}

D3D8Interface::~D3D8Interface()
{
    // TODO: Implement
}

RenderDevice3D::ECapsFlags D3D8Interface::GetCapsFlags()
{
    // TODO: Implement
    return (ECapsFlags)0;
}

void D3D8Interface::Cleanup()
{
    D3DInterface::Cleanup();

    // TODO: Implement
}

bool D3D8Interface::Flush(ulong flags)
{
    // TODO: Implement
    return false;
}

bool D3D8Interface::Present(const RECT *src, const RECT *dst)
{
    // TODO: Implement
    return false;
}

bool D3D8Interface::InitFromDDInterface(DDInterface *ddInterface, DDInterface::EResult *result, bool testOnly, bool *is3DRecommended)
{
    // TODO: Implement
    return false;
}

std::string D3D8Interface::GetInfoString(EInfoString infoType)
{
    // TODO: Implement
    return "";
}

void D3D8Interface::GetBackBufferDimensions(ulong &width, ulong &height)
{
    // TODO: Implement
    width = 0;
    height = 0;
}

IUnknown *D3D8Interface::CreateSurface(int width, int height, bool flush, bool forceARGB8)
{
    // TODO: Implement
    return nullptr;
}

bool D3D8Interface::CanBltSurface(bool unknown)
{
    // TODO: This function is missing in the binary, what's the intended implementation?
    return true;
}

void D3D8Interface::BltSurface(IUnknown *surface, const Rect& src, const Rect& dst)
{
    // TODO: Implement
}

void D3D8Interface::ClearColorBuffer(const Color &color)
{
    // TODO: Implement
}

D3D8Interface* Sexy::CreateD3D8Interface()
{
    return new D3D8Interface();
}