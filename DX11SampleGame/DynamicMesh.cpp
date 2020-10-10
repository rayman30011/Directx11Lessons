#include "DynamicMesh.h"

void DynamicMesh::build(ID3D11Device* device)
{
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * _data->vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.MiscFlags = 0;

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

Vertex* DynamicMesh::getVerticesAndLock(ID3D11DeviceContext* context, D3D11_MAP flag)
{
    D3D11_MAPPED_SUBRESOURCE mappedData;
    auto hr = context->Map(_vertexBuffer, 0, flag, 0, &mappedData);
    if (FAILED(hr))
        throw;

    Vertex* result = reinterpret_cast<Vertex*>(mappedData.pData);
    return result;
}

void DynamicMesh::unlockVertexBuffer(ID3D11DeviceContext* context)
{
    context->Unmap(_vertexBuffer, 0);
}
