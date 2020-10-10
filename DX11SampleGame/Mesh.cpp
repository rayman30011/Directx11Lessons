#include "Mesh.h"

Mesh::Mesh()
{
    _data = new MeshData;
    _scale = { 1.f, 1.f, 1.f };
    _position = { 0.f, 0.f, 0.f };
}

void Mesh::build(ID3D11Device* device)
{
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * _data->vertices.size();
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &_data->vertices[0];
    auto hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
    if (FAILED(hr))
        throw;

    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.ByteWidth = sizeof(_data->indices[0]) * _data->indices.size();
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = &_data->indices[0];
    hr = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
    if (FAILED(hr))
        throw;
}

Mesh::~Mesh()
{
    ReleaseCom(_vertexBuffer);
    ReleaseCom(_indexBuffer);

    delete _data;
}
