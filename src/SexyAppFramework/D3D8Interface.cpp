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

bool D3D8Interface::InitFromDDInterface(DDInterface *ddInterface, DDInterface::EResult *result, bool testOnly, bool *is3DRecommended)
{
    // TODO: Implement
    return false;
}

void D3D8Interface::GetBackBufferDimensions(ulong &width, ulong &height)
{
    // TODO: Implement
    width = 0;
    height = 0;
}

D3D8Interface* Sexy::CreateD3D8Interface()
{
    return new D3D8Interface();
}