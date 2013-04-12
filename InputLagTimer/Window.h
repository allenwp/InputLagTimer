#pragma once
#include "Setup.h"
#include "WindowManager.h"
#include "TimerModel.h"
#include <memory>

#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

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

  std::unique_ptr<DirectX::CommonStates>                           g_States;
  std::unique_ptr<DirectX::BasicEffect>                            g_BatchEffect;
  std::unique_ptr<DirectX::EffectFactory>                          g_FXFactory;
  std::unique_ptr<DirectX::GeometricPrimitive>                     g_Shape;
  std::unique_ptr<DirectX::Model>                                  g_Model;
  std::unique_ptr<DirectX::SpriteBatch>                            g_Sprites;
  std::unique_ptr<DirectX::SpriteFont>                             g_Font;
};
