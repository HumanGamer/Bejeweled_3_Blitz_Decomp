#include "D3D9Interface.h"

using namespace Sexy;

D3D9Interface::D3D9Interface()
{
    // TODO: Implement
}

D3D9Interface::~D3D9Interface()
{
    // TODO: Implement
}

RenderDevice3D::ECapsFlags D3D9Interface::GetCapsFlags()
{
    // TODO: Implement
    return (ECapsFlags)0;
}

void D3D9Interface::Cleanup()
{
    D3DInterface::Cleanup();

    // TODO: Implement
}

bool D3D9Interface::Flush(ulong flags)
{
    // TODO: Implement
    return false;
}

bool D3D9Interface::Present(const RECT *src, const RECT *dst)
{
    // TODO: Implement
    return false;
}

bool D3D9Interface::InitFromDDInterface(DDInterface *ddInterface, DDInterface::EResult *result, bool testOnly, bool *is3DRecommended)
{
    // TODO: Implement
    return false;
}

std::string D3D9Interface::GetInfoString(EInfoString infoType)
{
    // TODO: Implement
    return "";
}

void D3D9Interface::GetBackBufferDimensions(ulong &width, ulong &height)
{
    // TODO: Implement
    width = 0;
    height = 0;
}

IUnknown *D3D9Interface::CreateSurface(int width, int height, bool flush, bool forceARGB8)
{
    // TODO: Implement
    return nullptr;
}

bool D3D9Interface::CanBltSurface(bool unknown)
{
    return true;
}

void D3D9Interface::BltSurface(IUnknown *surface, const Rect &src, const Rect &dst)
{
    // TODO: Implement
}

void D3D9Interface::ClearColorBuffer(const Color &color)
{
    // TODO: Implement
}

D3D9Interface* Sexy::CreateD3D9Interface()
{
    return new D3D9Interface();
}
