#include "BaseApp.h"
#include "Resource.h"
#include "Utils.h"

#include <sstream>
#include <windowsx.h>

namespace 
{
    BaseApp* app = nullptr;
}

LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
    return app->msgLoop(hwnd, msg, wParam, lParam);
}

BaseApp::BaseApp() : _width(1280), _height(720)
{
    app = this;
}

void BaseApp::init(HINSTANCE hInstance)
{
    _hInst = hInstance;

    if (!initWindow())
        throw;

    if (!initDirectx())
        throw;
}

bool BaseApp::isKeyPressed()
{
    return false;
}

void BaseApp::dispose()
{
}

int BaseApp::run()
{
    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            _time.tick();
            calculateFps();
            update(_time.deltaTime());
            render();
        }
    }

    return (int)msg.wParam;
}

bool BaseApp::initWindow()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = mainWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = _hInst;
    wcex.hIcon = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_DX11SAMPLEGAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"MyWINDOW";
    wcex.hIconSm = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);


    _hWnd = CreateWindowW(L"MyWINDOW", L"Game1", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, _width, _height, nullptr, nullptr, _hInst, nullptr);

    if (!_hWnd)
        return false;

    ShowWindow(_hWnd, SW_SHOW);
    UpdateWindow(_hWnd);

    return true;
}

bool BaseApp::initDirectx()
{
    uint32_t flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL level;
    auto result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, nullptr, 0, D3D11_SDK_VERSION, &_device, &level, &_context);
    if (FAILED(result))
    {
        MessageBox(nullptr, L"Create D3D11Device failed", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (level != D3D_FEATURE_LEVEL_11_0)
    {
        MessageBox(0, L"Direct3D Feature Level 11 unsupported", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }


    DXGI_SWAP_CHAIN_DESC desc;
    desc.BufferDesc.Width = _width;
    desc.BufferDesc.Height = _height;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 1;
    desc.OutputWindow = _hWnd;
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    desc.Flags = 0;

    IDXGIDevice* giDevice = nullptr;
    result = _device->QueryInterface(__uuidof(IDXGIDevice), (void**)&giDevice);
    if (FAILED(result))
        return false;

    IDXGIAdapter* giAdapter = nullptr;
    result = giDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&giAdapter);
    if (FAILED(result))
        return false;

    IDXGIFactory* giFactory = nullptr;
    result = giAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&giFactory);
    if (FAILED(result))
        return false;

    result = giFactory->CreateSwapChain(_device, &desc, &_swapChain);
    if (FAILED(result))
        return false;

    ReleaseCom(giDevice);
    ReleaseCom(giAdapter);
    ReleaseCom(giFactory);

    onResize();

    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthClipEnable = true;

    result = _device->CreateRasterizerState(&rasterDesc, &_wireframeRS);
    if (FAILED(result))
        return false;

    D3D11_RASTERIZER_DESC rasterSolidDesc;
    ZeroMemory(&rasterSolidDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterSolidDesc.FillMode = D3D11_FILL_SOLID;
    rasterSolidDesc.CullMode = D3D11_CULL_NONE;
    rasterSolidDesc.FrontCounterClockwise = false;
    rasterSolidDesc.DepthClipEnable = true;
    rasterSolidDesc.ScissorEnable = false;

    result = _device->CreateRasterizerState(&rasterSolidDesc, &_solidRS);
    if (FAILED(result))
        return false;

    return true;
}

void BaseApp::onResize()
{
    ReleaseCom(_renderTarget);
    ReleaseCom(_depthStencil);
    ReleaseCom(_depthBuffer);

    if (FAILED(_swapChain->ResizeBuffers(1, _width, _height, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
        throw;

    ID3D11Texture2D* backBuffer;
    if (FAILED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
        throw;
    _device->CreateRenderTargetView(backBuffer, 0, &_renderTarget);
    ReleaseCom(backBuffer);

    D3D11_TEXTURE2D_DESC depthDesc;
    depthDesc.Width = _width;
    depthDesc.Height = _height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;

    HRESULT result = _device->CreateTexture2D(&depthDesc, nullptr, &_depthBuffer);
    if (FAILED(result))
        throw;

    result = _device->CreateDepthStencilView(_depthBuffer, nullptr, &_depthStencil);
    if (FAILED(result))
        throw;

    _context->OMSetRenderTargets(1, &_renderTarget, _depthStencil);

    _viewPort.TopLeftX = 0;
    _viewPort.TopLeftY = 0;
    _viewPort.Width = static_cast<float>(_width);
    _viewPort.Height = static_cast<float>(_height);
    _viewPort.MinDepth = 0.f;
    _viewPort.MaxDepth = 1.f;

    _context->RSSetViewports(1, &_viewPort);
}

void BaseApp::calculateFps()
{
    static int frameCount = 0;
    static float timeElapsed = 0.f;

    if ((_time.gameTime() - timeElapsed) > 1.f)
    {
        float fps = (float)frameCount;
        float mspf = 1000.f / fps;

        std::wostringstream outs;
        outs.precision(6);
        outs << L"FPS: " << fps << L" " << L"Time: " << mspf << L"ms";
        SetWindowText(_hWnd, outs.str().c_str());
    }
}

LRESULT BaseApp::msgLoop(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_MOUSEWHEEL:
    {
        short value = 0;
        value = GET_WHEEL_DELTA_WPARAM(wParam);
        onMouseWheel(value);
    }
        
        break;

    case WM_SIZE:
        _width = LOWORD(lParam);
        _height = HIWORD(lParam);

        if (_device != nullptr && !_isResizing)
            onResize();

        break;
    case WM_ENTERSIZEMOVE:
        _isResizing = true;
        break;
    case WM_EXITSIZEMOVE:
        _isResizing = false;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MOUSEMOVE:
        onMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
