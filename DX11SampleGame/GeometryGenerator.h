#pragma once
#include "framework.h"
#include "Mesh.h"

class GeometryGenerator
{
public:
	void createGrid(float width, float depth, uint32_t m, uint32_t n, MeshData& meshData);
	void createPlane(float width, float depth, uint32_t m, uint32_t n, MeshData& meshData, const XMFLOAT4& color);
	void createBox(float width, float height, float depth, const XMFLOAT4& color, MeshData& meshData);
	void createCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCound, uint32_t stackCount, MeshData& meshData, const XMFLOAT4& color);
	void createSphere(float radius, uint32_t subvisions, MeshData& meshData, const XMFLOAT4& color);
	float getHeight(float x, float z);

	float updateWawes(float x, float z, float dt);
private:
	XMFLOAT3 getHillNormal(float x, float z) const;
	void buildCylinderTopCap(float topRadius, float height, uint32_t sliceCound, uint32_t stackCount, MeshData& meshData, const XMFLOAT4& color);
	void buildCylinderBottomCap(float bottomRadius, float height, uint32_t sliceCound, uint32_t stackCount, MeshData& meshData, const XMFLOAT4& color);
	void subdivide(MeshData& meshData);
};

