module;

#include "BlakesEngine/Core/beAssert.h"
#include <d3d11.h>

module beRendering;

using namespace beRendering;

int beRendering::GetD3DTopology(Topology topology)
{
	switch (topology)
	{
		case Topology::Invalid: return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case Topology::TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case Topology::TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default: break;
	}
	BE_ASSERT(false);
	return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
