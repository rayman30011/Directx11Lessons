#pragma once
#include "framework.h"
#include "Utils.h"

struct Vertex 
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 textureCoords;
};

struct Material
{
	XMFLOAT4 ambient;
	XMFLOAT4 difuse;
	XMFLOAT4 specular;
	//XMFLOAT4 reflect;
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Material material;
};

class Mesh
{
public:
	Mesh();

	virtual void build(ID3D11Device* device);
	MeshData* getData() { return _data; }
	ID3D11Buffer* indexBuffer() { return _indexBuffer; }
	ID3D11Buffer* vertexBuffer() { return _vertexBuffer; }

	XMFLOAT3 getPosition() { return _position; }
	void setPosition(XMFLOAT3 value) { _position = value; }
	XMFLOAT3 getScale() { return _scale; }
	void setScale(XMFLOAT3 value) { _scale = value; }

	~Mesh();

protected:
	ID3D11Buffer* _indexBuffer;
	ID3D11Buffer* _vertexBuffer;
	MeshData* _data;

	XMFLOAT3 _scale;
	XMFLOAT3 _position;
};
