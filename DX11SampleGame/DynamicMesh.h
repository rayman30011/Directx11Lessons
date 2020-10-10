#pragma once
#include "Mesh.h"
class DynamicMesh : public Mesh
{
public:
	DynamicMesh() : Mesh() { };

	virtual void build(ID3D11Device* device) override;

	Vertex* getVerticesAndLock(ID3D11DeviceContext* context, D3D11_MAP flag);
	void unlockVertexBuffer(ID3D11DeviceContext* context);
};

