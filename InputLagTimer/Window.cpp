#include "stdafx.h"
#include "Window.h"
#include <assert.h>
#include "Config.h"
#include <stdio.h>

#define TIMER_VALUE_PADDING 10
#define COLUMN_SEPARATOR_WIDTH 15

/* TODO: make these static class variables you dummie. >_> */
TCHAR Window::windowClassName[] = _T("InputLagTimerWindowClassName");
int Window::windowCount = 0;
UINT Window::mMaxWidth = 0;
UINT Window::mMaxHeight = 0;

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
  PAINTSTRUCT ps;
  HDC hdc;

  // TODO: Handle ESC key?
  switch (message)
  {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    // TODO: Add any drawing code here...
    EndPaint(hWnd, &ps);
    break;
  case WM_SETCURSOR:
    SetCursor(NULL);
    return true;
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

int Window::getWindowCount()
{
  return windowCount;
}

UINT Window::getMaxWidth()
{
  return mMaxWidth;
}

UINT Window::getMaxHeight()
{
  return mMaxHeight;
}

struct InsensitiveCompare
{ 
  bool operator() (const std::wstring& a, const std::wstring& b) const
  {
    return _wcsnicmp(a.c_str(), b.c_str(), 100) < 0;
  }
};

std::set<std::wstring, InsensitiveCompare>* Window::getFontPaths(const std::wstring& rootPath, bool* outError)
{
  std::set<std::wstring, InsensitiveCompare>* result = new std::set<std::wstring, InsensitiveCompare>();
  if(outError)
  {
    *outError = false;
  }

  /* Add "/*" to the end of the string to get all the files */
  std::wstring filenameSearch = rootPath;
  wchar_t lastChar = *rootPath.rbegin();
  if(lastChar != '/' && lastChar != '\\')
  {
    filenameSearch += '/';
  }
  filenameSearch += '*';

  WIN32_FIND_DATA ffd;
  HANDLE hFind = FindFirstFile(filenameSearch.c_str(), &ffd);
  if (INVALID_HANDLE_VALUE == hFind) 
  {
    if(outError)
    {
      *outError = true;
    }
  }
  else
  {
    do
    {
      std::wstring fileName = std::wstring(ffd.cFileName);
      if((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
      {
        result->insert(fileName);
      }
    }
    while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
  }

  return result;
}

Window::Window(HINSTANCE hInstance, const Setup::OutputSetting& outputSettings, const WindowManager::Device& device)
  :mModel(nullptr)
{
  mBufferDesc = outputSettings.bufferDesc;
  mDXGIOutput = outputSettings.output;
  mDXGIOutput->AddRef();

  IDXGIFactory* factory = NULL;
  CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&factory));
  
  /* Create the window */
  windowCount++;
  mWindowNumber = windowCount;

  mWindowName = new TCHAR[256]; /* 256 is max window name length */
  wsprintf(mWindowName, _T("Input Lag Timer - Output %i"), mWindowNumber);

  HWND hWnd = CreateWindow(windowClassName,
    mWindowName,
    WS_OVERLAPPEDWINDOW,
    outputSettings.windowPositionLeft,
    outputSettings.windowPositionTop,
    outputSettings.bufferDesc.Width,
    outputSettings.bufferDesc.Height,
    NULL,
    NULL,
    hInstance,
    NULL);

  // TODO: Could do something like this if I want, but I don't think it's necessary
  //if(windowCount == 1)
  //{
  //  g_pDXGIFactory->MakeWindowAssociation( p_Window->hWnd, 0 );
  //}

  ShowWindow(hWnd, SW_SHOWNORMAL);
  UpdateWindow(hWnd);

  /* Create the full-screen viewport */
  mViewport.Width = mBufferDesc.Width;
  mViewport.Height = mBufferDesc.Height;
  mViewport.MinDepth = 0.0f;
  mViewport.MaxDepth = 1.0f;
  mViewport.TopLeftX = 0;
  mViewport.TopLeftY = 0;
  
  /* Create the Swap Chain */
  IDXGIDevice* dxgiDevice = NULL;
  device.d3DDevice->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);

  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
  swapChainDesc.BufferDesc = outputSettings.bufferDesc;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.BufferCount = 1;
  swapChainDesc.OutputWindow = hWnd;
  swapChainDesc.Windowed = TRUE;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
  swapChainDesc.Flags = 0; // TODO: Should be DXGI_SWAP_CHAIN_FLAG_NONPREROTATED if I can get it working...

  factory->CreateSwapChain(dxgiDevice, &swapChainDesc, &mSwapChain);

  /* This shouldn't vary, but just in case, re-read the swap chain description
     so that we know exactly what it ended up being. */
  ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
  mSwapChain->GetDesc(&swapChainDesc);

  /* Create the Render Target */
  ID3D11Texture2D* backBuffer = NULL;
  mSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
  device.d3DDevice->CreateRenderTargetView(backBuffer, NULL, &mRenderTargetView);
  backBuffer->Release();

  dxgiDevice->Release();

  /* Remember the max width and height for rendering. */
  if(swapChainDesc.BufferDesc.Width > mMaxWidth)
  {
    mMaxWidth = swapChainDesc.BufferDesc.Width;
  }
  if(swapChainDesc.BufferDesc.Height > mMaxHeight)
  {
    mMaxHeight = swapChainDesc.BufferDesc.Height;
  }

  /* DirectX Toolkit setup */
  mSpriteBatch.reset( new DirectX::SpriteBatch( device.d3DDeviceConext ) );
  
  mPrimitiveBatch.reset(new DirectX::PrimitiveBatch<DirectX::VertexPositionColor>(device.d3DDeviceConext));
  mBasicEffect.reset(new DirectX::BasicEffect(device.d3DDevice));

  mBasicEffect->SetProjection(DirectX::XMMatrixOrthographicOffCenterRH(0, mBufferDesc.Width, mBufferDesc.Height, 0, 0, 1));
  mBasicEffect->SetVertexColorEnabled(true);

  void const* shaderByteCode;
  size_t byteCodeLength;
  mBasicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
  
  device.d3DDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
                            DirectX::VertexPositionColor::InputElementCount,
                            shaderByteCode, byteCodeLength,
                            &mInputLayout);

  /* Load fonts */
  std::wstring path = L"res/fonts/timer/";
  bool error;
  std::set<std::wstring, InsensitiveCompare>* fontPaths = getFontPaths(path.c_str(), &error);
  for(auto iter = fontPaths->begin(); iter != fontPaths->end(); ++iter)
  {
    std::wstring fullPath = path;
    fullPath.append(*iter);
    DirectX::SpriteFont* spriteFont = new DirectX::SpriteFont( device.d3DDevice, fullPath.c_str() );
    mSpriteFonts.push_back(spriteFont);
  }
  delete fontPaths;
  mSpriteFontNormal = new DirectX::SpriteFont( device.d3DDevice, L"res/fonts/normal.spritefont" );

  /* Maybe I want this in the future? Texture loading: */
  //CreateDDSTextureFromFile( device.d3DDevice, L"seafloor.dds", nullptr, &g_pTextureRV1 );
}


Window::~Window(void)
{
  if(mModel)
  {
    delete mModel;
  }
  mSwapChain->SetFullscreenState(FALSE, NULL);
  mSwapChain->Release();
  mRenderTargetView->Release();
  mDXGIOutput->Release();
  for(auto iter = mSpriteFonts.begin(); iter != mSpriteFonts.end(); ++iter)
  {
    delete *iter;
  }
  delete mSpriteFontNormal;
  delete mWindowName;
}

void Window::setFullscreen(BOOL fullscreen)
{
  mSwapChain->SetFullscreenState(fullscreen, mDXGIOutput);
}

void Window::initializeModel(const LARGE_INTEGER& startingCount, IDXGISwapChain* swapChain)
{
  mModel = new Model(startingCount, swapChain);
}

void Window::render(const WindowManager::Device& device)
{
  device.d3DDeviceConext->RSSetViewports(1, &mViewport);
  device.d3DDeviceConext->OMSetRenderTargets(1, &mRenderTargetView, NULL);

  device.d3DDeviceConext->ClearRenderTargetView( mRenderTargetView, Config::backgroundColour );
  
  mModel->update();
  renderModel(mModel, device);
  mSwapChain->Present( 0, 0 );
  mModel->renderComplete();
}

void Window::renderModel(Model* model, const WindowManager::Device& device)
{
  /* Generate strings */
  Model::TimerValue timerValue = model->getTimerValue();
  assert(timerValue.high < 1000 && timerValue.low < 100); /* Current design of display expects to always have less than a second */
  wchar_t timerString[7];
  swprintf_s(timerString, L"%03d.%02d", timerValue);

  /* Render sprites */
  // TODO: Determine which of deferred or immediate gives the least latency.
  mSpriteBatch->Begin( DirectX::SpriteSortMode_Deferred );
  auto fontIter = mSpriteFonts.begin();
  unsigned int x = TIMER_VALUE_PADDING;
  while(x < mMaxWidth && fontIter != mSpriteFonts.end())
  {
    int column = model->getColumn();
    x = drawColumn(timerString, x, column, *fontIter);

    ++fontIter;
  }
  mSpriteBatch->End();

  drawHUD(device);

  /* Render error if there is one */
  Model::ErrorType currentError = mModel->getCurrentError();
  if(Model::ERROR_TYPE_NONE != currentError)
  {
    float ClearColor[4] = { 1.0, 0.0, 0.0, 1.0f };
    device.d3DDeviceConext->ClearRenderTargetView( mRenderTargetView, ClearColor );
    
    mSpriteBatch->Begin( DirectX::SpriteSortMode_Deferred );
    std::wstring errorMessage;
    switch (currentError)
    {
    case Model::ERROR_TYPE_NONE:
      break;
    case Model::ERROR_TYPE_COUNTER_OVERFLOW:
      errorMessage = L"Performance counter has overflowed.\nPlease close and re-open this program.";
      break;
    case Model::ERROR_TYPE_RENDER_TIME_VARIANCE_TOO_HIGH:
      errorMessage = L"Jitter (render time variance) too high.\nWaiting for stability...";
      break;
    case Model::ERROR_TYPE_ACCURACY_TOO_LOW:
      errorMessage = L"Timer accuracy too low.\nWaiting for stability...";
      break;
    }
    mSpriteFontNormal->DrawString( mSpriteBatch.get(), errorMessage.c_str(), DirectX::XMFLOAT2(10 , 10), DirectX::Colors::White);
    mSpriteBatch->End();
  }
}

int Window::drawColumn(const wchar_t* timerString, int x, int column, DirectX::SpriteFont* font, bool drawHeader)
{
  unsigned int y = TIMER_VALUE_PADDING;
  DirectX::XMVECTOR textSize = font->MeasureString(L"888.88");
  int textWidth = static_cast<int>(ceilf(textSize.m128_f32[0])); // TODO: figure out how to access stuff via .x property. :S
  int lineHeight = static_cast<int>(ceilf(textSize.m128_f32[1]));

  /* Draw header */
  if(drawHeader)
  {
    font->DrawString( mSpriteBatch.get(), L"12345.67890", DirectX::XMFLOAT2(static_cast<float>(x) , static_cast<float>(y)), Config::fontColour);
    y += lineHeight + TIMER_VALUE_PADDING;
  }
  
  /* Draw Timer Values */
  int textX = x + (textWidth * column);
  while(y < getMaxHeight())
  {
    font->DrawString( mSpriteBatch.get(), timerString, DirectX::XMFLOAT2(static_cast<float>(textX) , static_cast<float>(y)), Config::fontColour);
    y += lineHeight + TIMER_VALUE_PADDING;
  }
  
  /* Draw Column Separator */
  int separatorX = x + (textWidth * Config::numColumns) + COLUMN_SEPARATOR_WIDTH;

  return separatorX;
}

void Window::drawHUD(const WindowManager::Device& device)
{
  wchar_t buffer[250];
  /*
  output1/2
  1920x1080
  59.97Hz

  542FPS

  accuracy
  +/-5.23ms
  error at
  +/-10.0ms

  jitter
  2.45ms
  error at
  2.0ms

  inputlag
  .allenwp
  .com

  FUTURE:
  time drift
  0.00045ms/1ms
  max accuracy
  +/-0.01ms
  */
  _snwprintf_s(buffer, 250, L"output%d/%d\n%dx%d\n%.2fHz\n\n%dFPS\n\naccuracy\n+/-%.2fms\nerror at\n+/-%.1fms\n\njitter\n%.2fms\nerror at\n%.1fms\n\nv0.8.0\n\ninputlag\n.allenwp\n.com",
    mWindowNumber, windowCount, mBufferDesc.Width, mBufferDesc.Height, static_cast<float>(mBufferDesc.RefreshRate.Numerator / mBufferDesc.RefreshRate.Denominator),
    mModel->getFPS(),
    static_cast<float>(mModel->getAccuracy() * 1000.0f), static_cast<float>(Config::lowestAccuracy * 1000.0f),
    static_cast<float>(mModel->getJitter() * 1000.0f), static_cast<float>(Config::highestJitter * 1000.0f));
  DirectX::XMVECTOR textSize = mSpriteFontNormal->MeasureString(buffer);

  mBasicEffect->Apply(device.d3DDeviceConext);
  device.d3DDeviceConext->IASetInputLayout(mInputLayout);

  float left = mBufferDesc.Width - textSize.m128_f32[0];
  float right = mBufferDesc.Width;
  float top = 0.0f;
  float bottom = textSize.m128_f32[1];
  DirectX::VertexPositionColor v1(DirectX::XMFLOAT3(left, top, 0.0), DirectX::XMFLOAT4(0.0,0.0,0.0,1.0));
  DirectX::VertexPositionColor v2(DirectX::XMFLOAT3(right, top, 0.0), DirectX::XMFLOAT4(0.0,0.0,0.0,1.0));
  DirectX::VertexPositionColor v3(DirectX::XMFLOAT3(right, bottom, 0.0), DirectX::XMFLOAT4(0.0,0.0,0.0,1.0));
  DirectX::VertexPositionColor v4(DirectX::XMFLOAT3(left, bottom, 0.0), DirectX::XMFLOAT4(0.0,0.0,0.0,1.0));

  mPrimitiveBatch->Begin();
  mPrimitiveBatch->DrawQuad(v1, v2, v3, v4);
  mPrimitiveBatch->End();

  mSpriteBatch->Begin( DirectX::SpriteSortMode_Deferred );
  mSpriteFontNormal->DrawString( mSpriteBatch.get(), buffer, DirectX::XMFLOAT2(left , top), DirectX::Colors::White);
  mSpriteBatch->End();
}

IDXGISwapChain* Window::getSwapChain()
{
  return mSwapChain;
}

Model* Window::getModel()
{
  return mModel;
}
