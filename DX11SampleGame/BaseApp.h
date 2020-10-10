#pragma once
#include "framework.h"
#include "Time.h"

class BaseApp
{
public:
	BaseApp();
	
	float aspectRatio() { return (float)_width / _height; }

	virtual void init(HINSTANCE hInstance);
	virtual void update(float time) = 0;
	virtual void render() = 0;
	virtual void onResize();
	virtual void onMouseMove(WPARAM wparam, uint32_t x, uint32_t y) {}
	virtual void onMouseWheel(short delta) {};
	virtual void onKeyPress() {}
	bool isKeyPressed();

	void dispose();
	int run();

	LRESULT msgLoop(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
private:
	bool initWindow();
	bool initDirectx();

	void calculateFps();

protected:
	ID3D11Device* _device;
	ID3D11DeviceContext* _context;
	IDXGISwapChain* _swapChain;
	ID3D11RenderTargetView* _renderTarget;
	ID3D11DepthStencilView* _depthStencil;
	ID3D11Texture2D* _depthBuffer;
	ID3D11InputLayout* _inputLayout;
	ID3D11RasterizerState* _wireframeRS;
	ID3D11RasterizerState* _solidRS;
	D3D11_VIEWPORT _viewPort;
	Time _time;
	POINT _lastMousePosition;

	HWND _hWnd;
	HINSTANCE _hInst;

	int _width;
	int _height;

	bool _isResizing;
};

