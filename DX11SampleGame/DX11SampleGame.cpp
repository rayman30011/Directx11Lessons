// DX11SampleGame.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "DX11SampleGame.h"
#include "App.h"


int APIENTRY wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    App app;
    app.init(hInstance);

    return app.run();
}