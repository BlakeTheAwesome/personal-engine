#ifndef _beShaderCBufferDefintions_h_
#define _beShaderCBufferDefintions_h_

#ifdef __cplusplus
#include "BlakesEngine/Math/beMath.h"
#define cbuffer struct
#define float4 Vec4
#define float3 Vec3
#define float2 Vec2
#define matrix Matrix
#define SlotId(id)
#define Semantic(s)
#else

#define SlotId(id) : register(b##id)
#define Semantic(s) : s

#endif //__cplusplus



#define CBUFIDX_MatrixBuffer 0
#define CBUFIDX_OrthoMatrixBuffer 1
#define CBUFIDX_CameraBuffer 2
#define CBUFIDX_LightBuffer 3
#define CBUFIDX_PositionBuffer 4
#define CBUFIDX_ColourBuffer 5

#define CBUF_MatrixBuffer\
	cbuffer MatrixBuffer SlotId(CBUFIDX_MatrixBuffer)\
	{\
		matrix worldMatrix;\
		matrix viewMatrix;\
		matrix projectionMatrix;\
		matrix worldToScreenMatrix;\
	};

#define CBUF_OrthoMatrixBuffer\
	cbuffer OrthoMatrixBuffer SlotId(CBUFIDX_OrthoMatrixBuffer)\
	{\
		matrix orthoMatrixNormalised;\
		matrix orthoMatrixPixels;\
		float2 screenSize;\
		float2 padding;\
	};

#define CBUF_CameraBuffer\
	cbuffer CameraBuffer SlotId(CBUFIDX_CameraBuffer)\
	{\
		float3 cameraPosition;\
		float padding;\
	};

#define CBUF_LightBuffer\
	cbuffer LightBuffer SlotId(CBUFIDX_LightBuffer)\
	{\
		float4 ambientColour;\
		float4 diffuseColour;\
		float3 lightDirection;\
		float specularPower;\
		float4 specularColour;\
	};

#define CBUF_ColourBuffer\
	cbuffer ColourBuffer SlotId(CBUFIDX_ColourBuffer)\
	{\
		float4 colour;\
	};


#define CBUF_PositionBuffer\
	cbuffer PositionBuffer SlotId(CBUFIDX_PositionBuffer)\
	{\
		float4 colour;\
		float2 positionOffset;\
		float2 padding2;\
	};

#define CS_TextureVertexInputType(name)\
	struct name\
	{\
		float4 position Semantic(POSITION);\
		float3 normal Semantic(NORMAL);\
		float2 uv Semantic(TEXCOORD0);\
	};
#define CS_Texture2DVertexInputType(name)\
	struct name\
	{\
		float2 position Semantic(POSITION);\
		float2 uv Semantic(TEXCOORD0);\
	};

#define CS_TexturePixelInputType(name)\
	struct name\
	{\
		float4 position Semantic(SV_POSITION);\
		float4 colour Semantic(COLOR0);\
		float2 uv Semantic(TEXCOORD0);\
	};

#define CS_LightVertexInputType(name)\
	struct name\
	{\
		float4 position Semantic(POSITION);\
		float3 normal Semantic(NORMAL);\
		float2 tex Semantic(TEXCOORD0);\
	};

#define CS_LightPixelInputType(name)\
	struct name\
	{\
		float4 position Semantic(SV_POSITION);\
		float3 normal Semantic(NORMAL);\
		float2 tex Semantic(TEXCOORD0);\
		float3 viewDirection Semantic(TEXCOORD1);\
	};


#define CS_ColourVertexInputType(name)\
	struct name\
	{\
		float4 position Semantic(POSITION);\
		float4 colour Semantic(COLOR);\
	};

#define CS_ColourPixelInputType(name)\
	struct name\
	{\
		float4 position Semantic(SV_POSITION);\
		float4 colour Semantic(COLOR);\
	};


#ifdef __cplusplus
namespace beShaderDefinitions {
	CBUF_OrthoMatrixBuffer;
	CBUF_MatrixBuffer;
	CBUF_CameraBuffer;
	CBUF_LightBuffer;
	CBUF_ColourBuffer;
	CBUF_PositionBuffer;

	namespace ShaderTexture
	{
		CS_TextureVertexInputType(VertexInputType);
		CS_TexturePixelInputType(PixelInputType);
	}
	namespace ShaderTexture2d
	{
		CS_Texture2DVertexInputType(VertexInputType);
		CS_TexturePixelInputType(PixelInputType);
	}

	namespace ShaderLitTexture
	{
		CS_LightVertexInputType(VertexInputType);
		CS_LightPixelInputType(PixelInputType);
	}

	namespace ShaderColour
	{
		CS_ColourVertexInputType(VertexInputType);
		CS_ColourPixelInputType(PixelInputType);
	}
}

#undef CS_TextureVertexInputType
#undef CS_TexturePixelInputType
#undef CS_Texture2DVertexInputType
#undef CS_TexturePixelInputType
#undef CS_LightVertexInputType


#undef CBUF_OrthoMatrixBuffer
#undef CBUF_MatrixBuffer
#undef CBUF_CameraBuffer
#undef CBUF_LightBuffer
#undef CBUF_ColourBuffer
#undef CBUF_PositionBuffer

#undef cbuffer
#undef float4
#undef float3
#undef float2
#undef matrix
#undef SlotId
#undef Semantic
#endif //__cplusplus
#endif //_beShaderCBufferDefintions_h_
