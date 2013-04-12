#include "stdafx.h"
#include "Window.h"
#include <assert.h>

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
  
  /* Create the Render Target */
  ID3D11Texture2D* backBuffer = NULL;
  mSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
  device.d3DDevice->CreateRenderTargetView(backBuffer, NULL, &mRenderTargetView);
  backBuffer->Release();

  dxgiDevice->Release();

  /* Remember the max width and height for rendering. */
  if(outputSettings.bufferDesc.Width > mMaxWidth)
  {
    mMaxWidth = outputSettings.bufferDesc.Width;
  }
  if(outputSettings.bufferDesc.Height > mMaxHeight)
  {
    mMaxHeight = outputSettings.bufferDesc.Height;
  }

  /* DirectX Toolkit setup */
  g_States.reset( new DirectX::CommonStates( device.d3DDevice ) );
  g_Sprites.reset( new DirectX::SpriteBatch( device.d3DDeviceConext ) );
  g_FXFactory.reset( new DirectX::EffectFactory( device.d3DDevice ) );

  g_BatchEffect.reset( new DirectX::BasicEffect( device.d3DDevice ) );
  g_BatchEffect->SetVertexColorEnabled(true);

  {
    void const* shaderByteCode;
    size_t byteCodeLength;

    g_BatchEffect->GetVertexShaderBytecode( &shaderByteCode, &byteCodeLength );

    hr = device.d3DDevice->CreateInputLayout( VertexPositionColor::InputElements,
      VertexPositionColor::InputElementCount,
      shaderByteCode, byteCodeLength,
      &g_pBatchInputLayout );
    if( FAILED( hr ) )
      return hr;
  }

  g_Font.reset( new SpriteFont( device.d3DDevice, L"italic.spritefont" ) );

  g_Shape = GeometricPrimitive::CreateTeapot( device.d3DDeviceConext, 4.f, 8, false );

  g_Model = Model::CreateFromSDKMESH( device.d3DDevice, L"tiny.sdkmesh", *g_FXFactory, true );

  // Load the Texture
  hr = CreateDDSTextureFromFile( device.d3DDevice, L"seafloor.dds", nullptr, &g_pTextureRV1 );
  if( FAILED( hr ) )
    return hr;

  hr = CreateDDSTextureFromFile( device.d3DDevice, L"windowslogo.dds", nullptr, &g_pTextureRV2 );
  if( FAILED( hr ) )
    return hr;

  // Initialize the world matrices
  g_World = XMMatrixIdentity();

  // Initialize the view matrix
  XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -6.0f, 0.0f );
  XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
  XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
  g_View = XMMatrixLookAtLH( Eye, At, Up );

  g_BatchEffect->SetView( g_View );

  // Initialize the projection matrix
  g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f );

  g_BatchEffect->SetProjection( g_Projection );
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

void Window::initializeModel(const LARGE_INTEGER& startingCount)
{
  mModel = new Model(startingCount);
}

void Window::render(const WindowManager::Device& device)
{
  device.d3DDeviceConext->OMSetRenderTargets(1, &mRenderTargetView, NULL);

  // TODO: I don't think I need this in full screen... Figure out if I do or not...
  //D3D11_VIEWPORT viewport;
  //viewport.TopLeftX = 0;
  //viewport.TopLeftY = 0;
  //viewport.Width = mWidth;
  //viewport.Height = mHeight;
  //viewport.MinDepth = 0.0f;
  //viewport.MaxDepth = 1.0f;
  //device.d3DDeviceConext->RSSetViewports(1, &viewport);

  // Just clear the backbuffer
  float red = (double)rand() / (double)RAND_MAX;
  float green = (double)rand() / (double)RAND_MAX;
  float blue = (double)rand() / (double)RAND_MAX;
  float ClearColor[4] = { red, green, blue, 1.0f }; //red,green,blue,alpha;
  if(windowNumber == 1)
  {
    ClearColor[0] = 1.0;
  }
  else
  {
    ClearColor[1] = 1.0;
    ClearColor[2] = 1.0;
  }
  device.d3DDeviceConext->ClearRenderTargetView( mRenderTargetView, ClearColor );
  
  mModel->update();

  renderModel(mModel);
  
  mSwapChain->Present( 0, 0 );
}

void Window::renderModel(Model* model)
{
  Model::TimerValue timerValue = model->getTimerValue();
  assert(timerValue.high < 1000 && timerValue.low < 100); /* Current design of display expects to always have less than a second */
  wchar_t timerString[7];
  swprintf_s(timerString, L"%03d.%02d", timerValue);

  g_Sprites->Begin( DirectX::SpriteSortMode_Deferred );
  g_Font->DrawString( g_Sprites.get(), L"DirectXTK Simple Sample", DirectX::XMFLOAT2( 100, 10 ), DirectX::Colors::Yellow );
  g_Sprites->End();
}
