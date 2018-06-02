#pragma once

namespace beRendering
{
	enum class Topology : u8
	{
		Invalid,
		TriangleStrip,
		TriangleList,
		Count
	};

	int GetD3DTopology(Topology topology);
}
