#pragma once
#include "Setup.h"
#include "WindowManager.h"

class Window
{
public:
  static ATOM registerWindow(HINSTANCE hInstance);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  static TCHAR windowClassName[];

  Window(HINSTANCE hInstance, const Setup::OutputSetting& outputSettings, const WindowManager::Device& device);
  virtual ~Window(void);

  void setFullscreen(BOOL fullscreen);

  void render(const WindowManager::Device& device);

protected:
  static int windowCount;
  TCHAR* windowName;
  int windowNumber;
  IDXGIOutput* mDXGIOutput;
  IDXGISwapChain* mSwapChain;
  ID3D11RenderTargetView* mRenderTargetView;
};
