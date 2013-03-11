#include "stdafx.h"
#include "Window.h"
#include <assert.h>

#define MAX_LOADSTRING 100
bool windowClassRegistered = false;
bool windowCleanedUp = false;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

ATOM Window::RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

  // Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_INPUTLAGTIMER, szWindowClass, MAX_LOADSTRING);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= Window::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INPUTLAGTIMER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; // MAKEINTRESOURCE(IDC_INPUTLAGTIMER)
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  windowClassRegistered = true;

	return RegisterClassEx(&wcex);
}

HANDLE Window::CreateThread(HINSTANCE hInstance)
{
	return ::CreateThread(NULL, 0, Window::StartWindowThread, hInstance, 9, NULL);
}

//
//   FUNCTION: (formerly) InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
Window::Window(HINSTANCE hInstance)
  : g_pd3dDevice(NULL),
  g_pImmediateContext(NULL),
  g_pSwapChain(NULL),
  g_pRenderTargetView(NULL)
{
  /* Window::RegisterWndClass(HINSTANCE hInstance) must be called first! */
  assert(windowClassRegistered);

  mHInstance = hInstance;

    RECT rc = { 0, 0, 640, 480 };
  AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

  mHWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
    NULL );

   ShowWindow(mHWnd, SW_SHOWDEFAULT);

   // TODO: The Direct3D tutorial sample didn't have this call... and neither did the multiwindow code... figure out why...
   UpdateWindow(mHWnd);

  if( FAILED( InitDevice() ) )
  {
    CleanupDevice();
  }
}

Window::~Window(void)
{
  if(!windowCleanedUp)
  {
    CleanupDevice();
  }
}

DWORD WINAPI Window::StartWindowThread( __in  LPVOID lpParameter )
{
	Window window((HINSTANCE)lpParameter);

	window.MessageLoop();

	return 0;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the window.
//
//  WM_PAINT	- Paint the window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
    // TODO: The multitheaded GUI didn't handle this message type... Figure out why...
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

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Window::InitDevice()
{
  HRESULT hr = S_OK;

  RECT rc;
  GetClientRect( mHWnd, &rc );
  UINT width = rc.right - rc.left;
  UINT height = rc.bottom - rc.top;

  UINT createDeviceFlags = 0;
#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_DRIVER_TYPE driverTypes[] =
  {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT numDriverTypes = ARRAYSIZE( driverTypes );

  D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };
  UINT numFeatureLevels = ARRAYSIZE( featureLevels );

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory( &sd, sizeof( sd ) );
  sd.BufferCount = 1;
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = mHWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

  for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
  {
    g_driverType = driverTypes[driverTypeIndex];
    hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
      D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
    if( SUCCEEDED( hr ) )
      break;
  }
  if( FAILED( hr ) )
    return hr;

  // Create a render target view
  ID3D11Texture2D* pBackBuffer = NULL;
  hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
  if( FAILED( hr ) )
    return hr;

  hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
  pBackBuffer->Release();
  if( FAILED( hr ) )
    return hr;

  g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL );

  // Setup the viewport
  D3D11_VIEWPORT vp;
  vp.Width = (FLOAT)width;
  vp.Height = (FLOAT)height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  g_pImmediateContext->RSSetViewports( 1, &vp );

  return S_OK;
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void Window::CleanupDevice()
{
  if( g_pImmediateContext ) g_pImmediateContext->ClearState();

  if( g_pRenderTargetView ) g_pRenderTargetView->Release();
  if( g_pSwapChain ) g_pSwapChain->Release();
  if( g_pImmediateContext ) g_pImmediateContext->Release();
  if( g_pd3dDevice ) g_pd3dDevice->Release();

  windowCleanedUp = true;
}

void Window::MessageLoop()
{
	MSG msg;

  // Doing it with accelerators... Might want to bring this back at some point...
  //HACCEL hAccelTable;
	//hAccelTable = LoadAccelerators(mHInstance, MAKEINTRESOURCE(IDC_INPUTLAGTIMER));
	//// Main message loop:
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}

  // The way that the direct3d tutorial did it...
  //// Main message loop
  //MSG msg = {0};
  //while( WM_QUIT != msg.message )
  //{
  //  if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
  //  {
  //    TranslateMessage( &msg );
  //    DispatchMessage( &msg );
  //  }
  //  else
  //  {
  //    Render();
  //  }
  //}
  
	while( true )
	{
		while (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		if (msg.message == WM_QUIT)
			break;

    // temporary for testing
    mModel.update();
		Render();

		// Give other windows some time to process messages
    Sleep(1);
		// TODO: usleep(100ULL);
	}
}

//--------------------------------------------------------------------------------------
// Render the frame
//--------------------------------------------------------------------------------------
void Window::Render()
{
  // Just clear the backbuffer
  float red = (double)rand() / (double)RAND_MAX;
  float green = (double)rand() / (double)RAND_MAX;
  float blue = (double)rand() / (double)RAND_MAX;
  float ClearColor[4] = { red, green, blue, 1.0f }; //red,green,blue,alpha
  g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );
  g_pSwapChain->Present( 0, 0 );
}
