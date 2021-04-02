#pragma once
#include "BlakesEngine/Core/beString.h"
#include "beRenderBuffer.h"
#include "beTexture.h"
#include "BlakesEngine/DataStructures/beVector.h"

import beMath;
import beArray;

class beRenderInterface;
class beShaderPack;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
class bePackedData;

class beModel
{
public:

	struct LoadOptions
	{
		enum Swizz {swizX=0, swizY=1, swizZ=2};

		beRendering::Topology topology = beRendering::Topology::TriangleList;
		bool flipFaces = false;
		float scale = 1.f;
		beArray<int, 3> axesSwizzle ={swizX, swizY, swizZ};

		static LoadOptions Swizzle(Swizz x, Swizz y, Swizz z, bool flipFaces=false)
		{
			return LoadOptions{beRendering::Topology::TriangleList, flipFaces, 1.f, {x, y, z}};
		}
	};
	struct Mesh
	{
		beString m_name;
		beRenderBuffer m_indexBuffer;
		int m_materialIndex = -1;
		bool m_enabled = true;
	};
	struct Material
	{
		beString m_name;
		beTexture m_texture;
		std::optional<Vec3> m_ambientColour;
		std::optional<Vec3> m_diffuseColour;
		std::optional<Vec3> m_specularColour;
		std::optional<float> m_specularPower;
		std::optional<float> m_alpha;
		std::optional<int> m_illuminationMode;
		beRendering::ShaderType m_shader = beRendering::ShaderType::Default;

		float m_bumpMultiplier = 1.f;
		beString m_texAmbient;
		beString m_texDiffuse;
		beString m_texBump;
		beString m_texSpec;
		beString m_texEnv;
	};

	beModel() = default;
	~beModel() { BE_ASSERT(!m_vertexBuffer.IsValid()); }

	beModel(const beModel&) = delete;
	beModel(beModel&&) = delete;
	beModel& operator=(const beModel&) = delete;
	beModel& operator=(beModel&&) = delete;


	bool Init(beRenderInterface* ri, beShaderPack* shaderPack, const beString& textureFilename);
	bool InitWithFilename(beRenderInterface* ri, beShaderPack* shaderPack, const char* filename, const beString& textureFilename, const LoadOptions& loadOptions);
	bool InitFromBuffers(beRenderBuffer* vertexBuffer, std::span<Mesh> meshes, std::span<Material> materials);
	bool InitFromPackedData(beRenderInterface* ri, beShaderPack* shaderPack, bePackedData const& packedData);
	void Deinit();

	void SetMeshVisibility(const beStringView& meshName, bool visible);
	void Render(beRenderInterface* ri, beShaderPack* shaderPack, beRendering::ShaderType shaderOverride=beRendering::ShaderType::Default);

private:
	bool LoadTexture(beRenderInterface* ri, beShaderPack* shaderPack, const beStringView& textureFilename, const beStringView& additionalLoadDir);
	
	int GetNumMeshes() const { return m_meshes.Count(); }
	ID3D11ShaderResourceView* GetTexture(int meshIndex = 0) const;
	const beRenderBuffer& GetVertexBuffer() const { return m_vertexBuffer; }
	const beRenderBuffer& GetIndexBuffer(int meshIndex = 0) const { return m_meshes[meshIndex].m_indexBuffer; }

	int GetIndexCount(int meshIndex = 0);

	struct OBJFileInfo;

	bool ReadMaterialLine(const std::string& line);
	static bool ReadMeshLine(const std::string& line, OBJFileInfo* fileInfo);

	beRenderBuffer m_vertexBuffer;
	beVector<Mesh, 1> m_meshes;
	beVector<Material, 1> m_materials;
	bool VerifyHeader(bePackedData const& packedData);
};
