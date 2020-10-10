#include "App.h"
#include "Utils.h"
#include <random>

App::App() : BaseApp()
{
    _theta = 1.5f * XM_PI;
    _phi = 0.25f * XM_PI;
    _radius = 25.0f;
    _angle = 0;

    XMMATRIX i = XMMatrixIdentity();
    XMStoreFloat4x4(&_worldMatrix, i);
    XMStoreFloat4x4(&_viewMatrix, i);
    XMStoreFloat4x4(&_projMatrix, i);
    XMStoreFloat4x4(&_textureTransform, i);

    _directionLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f); 
    _directionLight.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    _directionLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    _directionLight.direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

    _pointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f); 
    _pointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    _pointLight.specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    _pointLight.att = XMFLOAT3(0.0f, 0.1f, 0.0f);
    _pointLight.range = 25.0f;

    _spotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    _spotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    _spotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    _spotLight.att = XMFLOAT3(1.0f, 0.0f, 0.0f);
    _spotLight.spot = 96.0f;
    _spotLight.range = 10000.0f;
}

void App::init(HINSTANCE hinst)
{
    BaseApp::init(hinst);

    buildGeomerty();
    buildFx();
    buildInputLayout();

    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, aspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&_projMatrix, P);

    _box.setPosition({ -1.f, 3.f, 0.f });
    _box.setScale({ 20.f, 1.f, 20.f });

    int rows = 5;
    float sizeZ = 18.f;
    float stepZ = sizeZ / rows;

    for (size_t i = 0; i < rows; i++)
    {
        float half = sizeZ / 2.f;
        _cylinderPositions.push_back({ 4.f , 5.5, (i * stepZ) - half + 1 });
        _cylinderPositions.push_back({ -6.f , 5.5, (i * stepZ) - half + 1 });
    }

    HRESULT hr = D3DX11CreateShaderResourceViewFromFile(_device, L"Images/WoodCrate01.dds", nullptr, nullptr, &_textureSRW, nullptr);
    if (FAILED(hr))
        throw;
}

void App::update(float time)
{
    float x = _radius * sinf(_phi) * cosf(_theta);
    float z = _radius * sinf(_phi) * sinf(_theta);
    float y = _radius * cosf(_phi);

    _cameraPosition.x = x;
    _cameraPosition.y = y;
    _cameraPosition.z = z;

    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&_viewMatrix, V);

    static float a = 0;
    a += time;

    _pointLight.position.x = 30.0f * cosf(0.2f * a); 
    _pointLight.position.z = 30.0f * sinf(0.2f * a);
    _pointLight.position.y = MAX(generator.getHeight(_pointLight.position.x, _pointLight.position.z), -3.0f) + 10.0f;

    _spotLight.position = _cameraPosition; 
    XMStoreFloat3(&_spotLight.direction, XMVector3Normalize(target - pos));

    D3D11_MAPPED_SUBRESOURCE mappedData;
    ZeroMemory(&mappedData, sizeof(D3D11_MAPPED_SUBRESOURCE));
    ID3D11Buffer* buf = _wawes.vertexBuffer();

    Vertex* result = _wawes.getVerticesAndLock(_context, D3D11_MAP_WRITE_NO_OVERWRITE);
    for (size_t i = 0; i < 50; i++)
    {
        for (size_t j = 0; j < 50; j++)
        {
            XMFLOAT3 pos = result[i * 10 + j].position;
            result[i * 50 + j].position.y = sin(pos.x + pos.z + a);
        }
    }
    _wawes.unlockVertexBuffer(_context);
}

void App::render()
{
    if (_swapChain && _context)
    {
        float ClearColor[4] = { 0.56f, 0.8f, 1.0f, 1.0f };
        _context->ClearRenderTargetView(_renderTarget, ClearColor);
        _context->ClearDepthStencilView(_depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

        D3D11_RECT rects = { 100, 100, 800, 800 }; 
        _context->RSSetScissorRects(1, &rects);

        _context->IASetInputLayout(_inputLayout);
        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _context->RSSetState(_solidRS);

        _fxDirectionLight->SetRawValue(&_directionLight, 0, sizeof(DirectionLight));
        _fxSpotLight->SetRawValue(&_spotLight, 0, sizeof(SpotLight));
        _fxPointLight->SetRawValue(&_pointLight, 0, sizeof(PointLight));
        _fxEyePosition->SetRawValue(&_cameraPosition, 0, sizeof(XMFLOAT3));

        _fxDiffuseMap->SetResource(_textureSRW);
        XMMATRIX texTransform = XMLoadFloat4x4(&_textureTransform);
        _fxTextureTransform->SetMatrix(reinterpret_cast<float*>(&_textureTransform));
        renderMesh(_box);
        renderMesh(_terrain);
        renderMesh(_wawes);

        for (const XMFLOAT3& position : _cylinderPositions)
        {
            _cylinder.setPosition(position);
            renderMesh(_cylinder);

            XMVECTOR pos = XMLoadFloat3(&position);
            XMFLOAT3 spherePosition;
            XMStoreFloat3(&spherePosition, pos + XMVECTOR({ 0, 2.f, 0 }));

            _sphere.setPosition(spherePosition);
            renderMesh(_sphere);
        }
        _swapChain->Present(0, 0);
    }
}

void App::renderMesh(Mesh& mesh) const
{
    XMFLOAT3 position = mesh.getPosition();
    XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
    XMFLOAT3 scale = mesh.getScale();
    XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

    XMMATRIX view = XMLoadFloat4x4(&_viewMatrix);
    XMMATRIX proj = XMLoadFloat4x4(&_projMatrix);
    XMMATRIX world =  scaleMatrix * translate;

    auto stride = sizeof(Vertex);
    UINT offset = 0;
    auto vertexBuffer = mesh.vertexBuffer();
    _context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    _context->IASetIndexBuffer(mesh.indexBuffer(), DXGI_FORMAT_R32_UINT, 0);

    D3DX11_TECHNIQUE_DESC desc;
    _tech->GetDesc(&desc);
    for (int i = 0; i < desc.Passes; ++i)
    {
        XMMATRIX worldViewProj = world * view * proj;
        _fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
        _fxMaterial->SetRawValue(&mesh.getData()->material, 0, sizeof(Material));

        XMMATRIX worldInvTranspose = Math::inverseTranspose(world);
        _fxWorldInv->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));

        _fxWorld->SetMatrix(reinterpret_cast<float*>(&world));

        _tech->GetPassByIndex(i)->Apply(0, _context);
        _context->DrawIndexed(mesh.getData()->indices.size(), 0, 0);
    }
}

void App::onMouseMove(WPARAM btnState, uint32_t x, uint32_t y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        float dx = (float)x - _lastMousePosition.x;
        float dy = (float)y - _lastMousePosition.y;

        _theta += -(dx * 0.01);
        _phi += -(dy * 0.01);
    }

    _lastMousePosition.x = x;
    _lastMousePosition.y = y;
}

void App::onMouseWheel(short value)
{
    _radius -= value / 30.f;
}

void App::buildInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    D3DX11_PASS_DESC passDesc; 
    _tech->GetPassByIndex(0)->GetDesc(&passDesc);

    auto hr = _device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_inputLayout);
    if (FAILED(hr))
        throw;
}

void App::buildGeomerty()
{
    generator.createGrid(100, 100, 50, 50, *_terrain.getData());
    _terrain.build(_device);

    generator.createBox(1, 1, 1, (float*)&Colors::Yellow, *_box.getData());
    _box.build(_device);

    generator.createCylinder(0.5f, 0.3f, 3.0f, 20, 20, *_cylinder.getData(), (float*)&Colors::White);
    _cylinder.build(_device);

    generator.createSphere(0.5f, 2, *_sphere.getData(), (float*)&Colors::Red);
    _sphere.build(_device);

    generator.createPlane(100, 100, 50, 50, *_wawes.getData(), (float*)&Colors::Blue);
    for (size_t i = 0; i < 50; i++)
    {
        for (size_t j = 0; j < 50; j++)
        {
            int rnd = rand() % 1000;
            float y = rnd / 1000.f;
            _wawes.getData()->vertices[i * 50 + j].position.y = y;
        }
    }

    _wawes.build(_device);

    _terrain.getData()->material.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    _terrain.getData()->material.difuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    _terrain.getData()->material.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 512.0f);

    _wawes.getData()->material.ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    _wawes.getData()->material.difuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    _wawes.getData()->material.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

    _box.getData()->material.ambient = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    _box.getData()->material.difuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    _box.getData()->material.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    _sphere.getData()->material.ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
    _sphere.getData()->material.difuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
    _sphere.getData()->material.specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

    _cylinder.getData()->material.ambient = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
    _cylinder.getData()->material.difuse = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
    _cylinder.getData()->material.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}

void App::buildFx()
{
    DWORD flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3D10_SHADER_DEBUG;
    flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    ID3D10Blob* compiledShader = nullptr;
    ID3D10Blob* compilationMsgs = nullptr;

    auto hr = D3DX11CompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, nullptr, "fx_5_0", flags, 0, nullptr, &compiledShader, &compilationMsgs, nullptr);
    if (compilationMsgs != 0) 
    { 
        MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0); 
        compilationMsgs->Release(); 
    }

    if (FAILED(hr)) throw;

    hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, _device, &_effect);
    if (FAILED(hr)) throw;

    _tech = _effect->GetTechniqueByName("ColorTech");
    _fxWorldViewProj = _effect->GetVariableByName("worldViewProj")->AsMatrix();
    _fxWorld = _effect->GetVariableByName("gWorld")->AsMatrix();
    _fxWorldInv = _effect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
    _fxTextureTransform = _effect->GetVariableByName("gTexTransform")->AsMatrix();
    _fxDirectionLight = _effect->GetVariableByName("gDirLight");
    _fxPointLight = _effect->GetVariableByName("gPointLight");
    _fxSpotLight = _effect->GetVariableByName("gSpotLight");
    _fxSpotLight = _effect->GetVariableByName("gSpotLight");
    _fxEyePosition = _effect->GetVariableByName("gEyePosW")->AsVector();
    _fxMaterial = _effect->GetVariableByName("gMaterial");
    _fxDiffuseMap = _effect->GetVariableByName("gDiffuseMap")->AsShaderResource();
}
