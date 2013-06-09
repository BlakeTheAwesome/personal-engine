#pragma once
#include "Math\beMath.h"

class beRenderInterface;
struct ID3D11Buffer;

class beModel
{
public:

	beModel();
	~beModel();

	bool Init(beRenderInterface* ri);
	void Deinit();

	void Render(beRenderInterface* ri);

	int GetIndexCount();

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
};
