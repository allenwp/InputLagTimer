#include "stdafx.h"
#include "Window.h"

TCHAR Window::windowClassName[] = _T("InputLagTimerWindowClassName");
int Window::windowCount = 0;

ATOM Window::registerWindow(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= Window::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= windowClassName;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

Window::Window(HINSTANCE hInstance, const Setup::OutputSetting& outputSettings)
{
  windowCount++;
  windowNumber = windowCount;

  windowName = new TCHAR[256]; /* 256 is max window name length */
  wsprintf(windowName, _T("Input Lag Timer - Output %i"), windowNumber); // TODO

  HWND hWnd = CreateWindow(windowClassName, windowName, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
  ShowWindow(hWnd, SW_SHOWNORMAL);
  UpdateWindow(hWnd);
}


Window::~Window(void)
{
  delete windowName;
}

void Window::render(const WindowManager::Device& device)
{
  // todo: something like this:

  //g_pD3DDeviceContext->OMSetRenderTargets( 1, &p_Window->p_RenderTargetView, NULL );

  //// Don't forget to adjust the viewport, in fullscreen it's not important...
  //D3D11_VIEWPORT Viewport;
  //Viewport.TopLeftX = 0;
  //Viewport.TopLeftY = 0;
  //Viewport.Width = p_Window->width;
  //Viewport.Height = p_Window->height;
  //Viewport.MinDepth = 0.0f;
  //Viewport.MaxDepth = 1.0f;
  //g_pD3DDeviceContext->RSSetViewports( 1, &Viewport );

  //// TO DO: AMAZING STUFF PER WINDOW
  //// Just clear the backbuffer
  //float red = (double)rand() / (double)RAND_MAX;
  //float green = (double)rand() / (double)RAND_MAX;
  //float blue = (double)rand() / (double)RAND_MAX;
  //float ClearColor[4] = { red, green, blue, 1.0f }; //red,green,blue,alpha;
  //if(i == 0)
  //{
  //  ClearColor[0] = 1.0;
  //}
  //else
  //{
  //  ClearColor[1] = 1.0;
  //  ClearColor[2] = 1.0;
  //}
  //g_pD3DDeviceContext->ClearRenderTargetView( p_Window->p_RenderTargetView, ClearColor );
  //p_Window->p_SwapChain->Present( 0, 0 );
}
