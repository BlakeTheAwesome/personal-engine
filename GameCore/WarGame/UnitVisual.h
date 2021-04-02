#pragma once
#include "BlakesEngine/Shaders/beShaderColour.h"
#include "BlakesEngine/DataStructures/beArray.h"
import beMath;

struct UnitVisual
{
	static constexpr int FacesPerBlock = 5;
	static constexpr int TrisPerFace = 2;
	static constexpr int VertsPerFace = 4;
	static constexpr int IndicesPerFace = TrisPerFace * 3;
	static constexpr int VertsPerBlock = FacesPerBlock * VertsPerFace;
	static constexpr int TrisPerBlock = FacesPerBlock * TrisPerFace;
	static constexpr int IndicesPerBlock = TrisPerBlock * 3;

	beArray<beShaderColour::VertexType, VertsPerBlock> verts;

	beMath::Vec3 BlockMin() const
 	{
		const Vec4& minVert = verts[0].position;
		return Vec3(minVert.x, minVert.y, minVert.z);
	}

	beMath::Vec3 BlockMax() const
	{
		const Vec4& maxVert = verts[6].position;
		return Vec3(maxVert.x, maxVert.y, maxVert.z);
	}
};