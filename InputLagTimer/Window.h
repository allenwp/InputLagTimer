#pragma once
#include "Setup.h"
#include "WindowManager.h"
#include "TimerModel.h"
#include <memory>
#include <set>

#include "SpriteBatch.h"
#include "SpriteFont.h"

struct InsensitiveCompare;

class Window
{
public:
  static ATOM registerWindow(HINSTANCE hInstance);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  static int getWindowCount();
  static UINT getMaxWidth();
  static UINT getMaxHeight();
  /**
   * @return the set of paths, including the root path, of the spritefont files found in rootPath.
   * YOU MUST CALL DELETE on the result of this function: transfer of ownership will occur.
   */
  static std::set<std::wstring, InsensitiveCompare>* Window::getFontPaths(const std::wstring& rootPath, bool* outError);

  Window(HINSTANCE hInstance, const Setup::OutputSetting& outputSettings, const WindowManager::Device& device);
  virtual ~Window(void);

  void setFullscreen(BOOL fullscreen);
  void initializeModel(const LARGE_INTEGER& startingCount, IDXGISwapChain* swapChain);
  void render(const WindowManager::Device& device);

  IDXGISwapChain* getSwapChain();

protected:
  void renderModel(Model* model, const WindowManager::Device& device);

  /**
   * @return the new x coordinate of the right of the column that was just drawn
   * @param drawHeader will draw "12345.67890" as reference digits at the top of the column.
   * The header was an old feature that was designed to help reability, but after the discovery
   * of varaible latancy displays, it was decided that the header would take away from otherwise
   * important values at the top of the column.
   */
  int Window::drawColumn(const wchar_t* timerString, int x, int column, DirectX::SpriteFont* font, bool drawHeader = false);

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

  std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;
  std::vector<DirectX::SpriteFont*> mSpriteFonts;
};
