#pragma once

#include "Resource.h"
#include "Model.h"
#include <d3d11.h>

class Window
{
public:
	static ATOM RegisterWndClass(HINSTANCE hInstance);
  static HANDLE CreateThread(HINSTANCE hInstance);
  Window(HINSTANCE hInstance);
  virtual ~Window(void);

private:
  static DWORD WINAPI StartWindowThread( __in  LPVOID lpParameter );
  static LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HRESULT InitDevice();
  void CleanupDevice();
  void MessageLoop();
  void Render();

  HINSTANCE mHInstance;
  HWND mHWnd;

  Model mModel;

  ID3D11Device*           g_pd3dDevice;
  ID3D11DeviceContext*    g_pImmediateContext;
  IDXGISwapChain*         g_pSwapChain;
  ID3D11RenderTargetView* g_pRenderTargetView;
};

