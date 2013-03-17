#ifndef _SETUP_H_

#include "stdafx.h"
#include <d3d11.h>

struct WindowDataContainer
{
    //Direct3D 11 stuff per window data
    IDXGISwapChain* p_SwapChain;
    ID3D11RenderTargetView* p_RenderTargetView;
    ID3D11DepthStencilView* p_DepthStencilView;

    // window goodies
    HWND hWnd;
    int width;
    int height;
};

void init(HINSTANCE hInst);
ATOM RegisterWndClass(HINSTANCE hInstance);
LRESULT CALLBACK SecondaryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void GetAdapter();
void EnumOutputsOnAdapter();
void CreateWindowsForOutputs();
void CreateSwapChainsAndViews();
void MultiRender( );

#endif