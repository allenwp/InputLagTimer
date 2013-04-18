#include "stdafx.h"
#include "Window.h"
#include <assert.h>

#define TIMER_VALUE_PADDING 10.0
#define TIMER_VALUE_COLOUR DirectX::Colors::Yellow
#define COLUMN_SEPARATOR_WIDTH 15.0

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
	int wmId, wmEvent;
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

Window::Window(HINSTANCE hInstance, const Setup::OutputSetting& outputSettings, const WindowManager::Device& device)
  :mModel(nullptr)
{
  mDXGIOutput = outputSettings.output;
  mDXGIOutput->AddRef();

  IDXGIFactory* factory = NULL;
  CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&factory));
  
  /* Create the window */
  windowCount++;
  windowNumber = windowCount;

  windowName = new TCHAR[256]; /* 256 is max window name length */
  wsprintf(windowName, _T("Input Lag Timer - Output %i"), windowNumber);

  HWND hWnd = CreateWindow(windowClassName,
    windowName,
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

  /* Setup the viewport */
  D3D11_VIEWPORT vp;
  vp.Width = swapChainDesc.BufferDesc.Width;
  vp.Height = swapChainDesc.BufferDesc.Height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  device.d3DDeviceConext->RSSetViewports( 1, &vp );

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

  mSpriteFont.reset( new DirectX::SpriteFont( device.d3DDevice, L"res/arabtype.spritefont" ) );

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
  delete windowName;
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
  device.d3DDeviceConext->OMSetRenderTargets(1, &mRenderTargetView, NULL);

  // Just clear the backbuffer
  //float red = (double)rand() / (double)RAND_MAX;
  //float green = (double)rand() / (double)RAND_MAX;
  //float blue = (double)rand() / (double)RAND_MAX;
  //float ClearColor[4] = { red, green, blue, 1.0f }; //red,green,blue,alpha;
  //if(windowNumber == 1)
  //{
  //  ClearColor[0] = 1.0;
  //}
  //else
  //{
  //  ClearColor[1] = 1.0;
  //  ClearColor[2] = 1.0;
  //}
  float ClearColor[4] = { 0.5, 0.5, 0.5, 1.0f };
  device.d3DDeviceConext->ClearRenderTargetView( mRenderTargetView, ClearColor );
  
  mModel->update();
  renderModel(mModel);
  mSwapChain->Present( 0, 0 );
  mModel->renderComplete();
}

void Window::renderModel(Model* model)
{
  /* Generate strings */
  Model::TimerValue timerValue = model->getTimerValue();
  assert(timerValue.high < 1000 && timerValue.low < 100); /* Current design of display expects to always have less than a second */
  wchar_t timerString[7];
  swprintf_s(timerString, L"%03d.%02d", timerValue);

  /* Render sprites */
  // TODO: Determine which of deferred or immediate gives the least latency.
  mSpriteBatch->Begin( DirectX::SpriteSortMode_Deferred );

  int x = TIMER_VALUE_PADDING;
  while(x < mMaxWidth)
  {
    int column = model->getColumn();
    x = drawColumn(timerString, x, column, *mSpriteFont);
  }

  mSpriteBatch->End();
}

int Window::drawColumn(const wchar_t* timerString, int x, int column, const DirectX::SpriteFont& font)
{
  int y = TIMER_VALUE_PADDING;
  DirectX::XMVECTOR textSize = font.MeasureString(L"888.88");
  int textWidth = ceilf(textSize.m128_f32[0]); // TODO: figure out how to access stuff via .x property. :S
  int lineHeight = ceilf(textSize.m128_f32[1]);

  /* Draw header */
  mSpriteFont->DrawString( mSpriteBatch.get(), L"12345.67890", DirectX::XMFLOAT2(x , y), TIMER_VALUE_COLOUR);
  y += lineHeight + TIMER_VALUE_PADDING;
  
  /* Draw Timer Values */
  int textX = x + (textWidth * column);
  while(y < getMaxHeight())
  {
    mSpriteFont->DrawString( mSpriteBatch.get(), timerString, DirectX::XMFLOAT2(textX , y), TIMER_VALUE_COLOUR);
    y += lineHeight + TIMER_VALUE_PADDING;
  }
  
  /* Draw Column Separator */
  int separatorX = x + (textWidth * 3) + COLUMN_SEPARATOR_WIDTH;

  return separatorX;
}

IDXGISwapChain* Window::getSwapChain()
{
  return mSwapChain;
}
