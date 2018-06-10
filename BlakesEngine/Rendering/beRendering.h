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

	enum class ShaderType
	{
		Invalid = -1,
		Default,
		LitTexture,
		Texture,
		Colour,
		//Texture2D,

		Count
	};

	int GetD3DTopology(Topology topology);
}
