#pragma once
#include "Setup.h"
#include "WindowManager.h"
#include "Model.h"

class Window
{
public:
  static ATOM registerWindow(HINSTANCE hInstance);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  static int getWindowCount();
  static UINT getMaxWidth();
  static UINT getMaxHeight();

  Window(HINSTANCE hInstance, const Setup::OutputSetting& outputSettings, const WindowManager::Device& device);
  virtual ~Window(void);

  void setFullscreen(BOOL fullscreen);
  void initializeModel(const LARGE_INTEGER& startingCount);
  void render(const WindowManager::Device& device);

protected:
  void renderModel(Model* model);

  static TCHAR windowClassName[];
  static int windowCount;
  static UINT mMaxWidth;
  static UINT mMaxHeight;
  TCHAR* windowName;
  int windowNumber;
  IDXGIOutput* mDXGIOutput;
  IDXGISwapChain* mSwapChain;
  ID3D11RenderTargetView* mRenderTargetView;
  Model* mModel;
};
