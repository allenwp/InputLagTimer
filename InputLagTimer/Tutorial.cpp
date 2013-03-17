#include "stdafx.h"
#include "Tutorial.h"
#include <vector>

IDXGIFactory* g_pDXGIFactory = NULL;
HINSTANCE g_hInstance;

TCHAR g_szWindowedName[] = L"something";					// The title bar text
TCHAR g_szWindowClass[] = L"Aclass";			// the main window class name

void init(HINSTANCE hInst)
{
  g_hInstance = hInst;
  CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&g_pDXGIFactory) );
  RegisterWndClass(hInst);
}

ATOM RegisterWndClass(HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style	= CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc	= SecondaryWndProc;
  wc.cbClsExtra	= 0;
  wc.cbWndExtra	= 0;
  wc.hInstance	= hInstance;
  wc.hIcon	= NULL;
  wc.hCursor	= LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName	= NULL; // MAKEINTRESOURCE(IDC_INPUTLAGTIMER)
  wc.lpszClassName	= g_szWindowClass;

  return RegisterClass(&wc);
}

LRESULT CALLBACK SecondaryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

IDXGIAdapter* g_pDXGIAdapter = NULL; // set to null, init by yourself
void GetAdapter() // applicable for multiple ones with little effort
{
    for( int i = 0; /* manual breakout */ ; i++ )
    {
        // remember, we assume there's only one adapter (example purposes)
        HRESULT hr = g_pDXGIFactory->EnumAdapters( i, &g_pDXGIAdapter );
        if( hr == DXGI_ERROR_NOT_FOUND ) // NOT equiv. to FAILED, a legit. check
            break; // end, there's no second adapter

        // get the description of the adapter, assuming no failure
        DXGI_ADAPTER_DESC AdapterDesc;
        hr = g_pDXGIAdapter->GetDesc( &AdapterDesc );

        // Getting the outputs active on our adapter
        EnumOutputsOnAdapter();
    }
}

std::vector<IDXGIOutput*> OutputArray; // contains outputs per adapter
void EnumOutputsOnAdapter()
{
    for( int i = 0; ; i++ )
    {
        IDXGIOutput* p_Output;
        HRESULT hr = g_pDXGIAdapter->EnumOutputs( i, &p_Output );
        if( DXGI_ERROR_NOT_FOUND == hr )
            break; // found them all, no more!

        // get the description
        DXGI_OUTPUT_DESC OutputDesc;
        hr = p_Output->GetDesc( &OutputDesc );

        OutputArray.push_back( p_Output );
    }

}

std::vector<WindowDataContainer*> WindowsArray;
void CreateWindowsForOutputs()
{
    for( int i = 0; i < OutputArray.size(); i++ )
    {

        IDXGIOutput* p_Output = OutputArray.at(i);
        DXGI_OUTPUT_DESC OutputDesc;
        p_Output->GetDesc( &OutputDesc );
        int x = 0;//OutputDesc.DesktopCoordinates.left;
        int y = 0;//OutputDesc.DesktopCoordinates.top;
        int width = 1024;//OutputDesc.DesktopCoordinates.right - x;
        int height = 768;//OutputDesc.DesktopCoordinates.bottom - y;

        WindowDataContainer *p_Window = new WindowDataContainer;

        p_Window->hWnd = CreateWindow( g_szWindowClass,
                                        g_szWindowedName,
                                        WS_OVERLAPPEDWINDOW,
                                        x,
                                        y,
                                        width,
                                        height,
                                        NULL,
                                        0,
                                        g_hInstance,
                                        NULL );
        DWORD error = GetLastError();
        // show the window
        ShowWindow( p_Window->hWnd, SW_SHOWDEFAULT );

        // set width and height
        p_Window->width = width;
        p_Window->height = height;

        // shove it in the std::vector
        WindowsArray.push_back( p_Window );

        //if first window, associate it with DXGI so it can jump in
        // when there is something of interest in the message queue
        // think fullscreen mode switches etc. MSDN for more info.
        if(i == 0)
            g_pDXGIFactory->MakeWindowAssociation( p_Window->hWnd, 0 );

    }
}

ID3D11Device* g_pD3DDevice = NULL;
ID3D11DeviceContext* g_pD3DDeviceContext = NULL;
void CreateSwapChainsAndViews()
{
  /* Any and all. I don't need anything more than a 9_1 */
  D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
  };
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
  D3D_FEATURE_LEVEL* featureLevel = nullptr;

  D3D11CreateDevice(g_pDXGIAdapter, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,  &g_pD3DDevice, featureLevel, &g_pD3DDeviceContext);

    for( int i = 0; i < WindowsArray.size(); i++ )
    {
        WindowDataContainer* p_Window = WindowsArray.at(i);

        // get the dxgi device
        IDXGIDevice* pDXGIDevice = NULL;
        HRESULT result = g_pD3DDevice->QueryInterface( IID_IDXGIDevice, ( void** )&pDXGIDevice ); // COM stuff, hopefully you are familiar

        // create a swap chain
        DXGI_SWAP_CHAIN_DESC SwapChainDesc;
        
        ZeroMemory( &SwapChainDesc, sizeof( SwapChainDesc ) );
        SwapChainDesc.BufferCount = 1;
        SwapChainDesc.BufferDesc.Width = p_Window->width;
        SwapChainDesc.BufferDesc.Height = p_Window->height;
        SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SwapChainDesc.OutputWindow = p_Window->hWnd;
        SwapChainDesc.SampleDesc.Count = 1;
        SwapChainDesc.SampleDesc.Quality = 0;
        SwapChainDesc.Windowed = TRUE;

        HRESULT hr = g_pDXGIFactory->CreateSwapChain( pDXGIDevice, &SwapChainDesc, &p_Window->p_SwapChain );

         // get the backbuffer
        ID3D11Texture2D* p_BackBuffer = NULL;
        hr = p_Window->p_SwapChain->GetBuffer( 0, IID_ID3D11Texture2D, ( void** )&p_BackBuffer );

        // get the backbuffer desc
        D3D11_TEXTURE2D_DESC BackBufferDesc;
        p_BackBuffer->GetDesc( &BackBufferDesc );

        // create the render target view
        //D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;

        // fill it in

        g_pD3DDevice->CreateRenderTargetView( p_BackBuffer, NULL, &p_Window->p_RenderTargetView );
        p_BackBuffer->Release();
        p_BackBuffer = NULL;

        //// Create depth stencil texture
        //ID3D11Texture2D* p_DepthStencil = NULL;
        //D3D11_TEXTURE2D_DESC descDepth;

        //// fill it in


        //g_pD3DDevice->CreateTexture2D( &descDepth, NULL, &p_DepthStencil );

        //// Create the depth stencil view
        //D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

        //// fill it in

        //g_pD3DDevice->CreateDepthStencilView( p_DepthStencil, &descDSV, &p_Window->p_DepthStencilView );

        pDXGIDevice->Release();
        pDXGIDevice = NULL;
    }

}

void MultiRender( )
{
    // Clear them all
    for( int i = 0; i < WindowsArray.size(); i++ )
    {
        WindowDataContainer* p_Window = WindowsArray.at(i);

        // There is the answer to your second question:
        g_pD3DDeviceContext->OMSetRenderTargets( 1, &p_Window->p_RenderTargetView, NULL );

        // Don't forget to adjust the viewport, in fullscreen it's not important...
        D3D11_VIEWPORT Viewport;
        Viewport.TopLeftX = 0;
        Viewport.TopLeftY = 0;
        Viewport.Width = p_Window->width;
        Viewport.Height = p_Window->height;
        Viewport.MinDepth = 0.0f;
        Viewport.MaxDepth = 1.0f;
        g_pD3DDeviceContext->RSSetViewports( 1, &Viewport );

        // TO DO: AMAZING STUFF PER WINDOW
        // Just clear the backbuffer
        float red = (double)rand() / (double)RAND_MAX;
        float green = (double)rand() / (double)RAND_MAX;
        float blue = (double)rand() / (double)RAND_MAX;
        float ClearColor[4] = { red, green, blue, 1.0f }; //red,green,blue,alpha;
        if(i == 0)
        {
          ClearColor[0] = 1.0;
        }
        else
        {
          ClearColor[1] = 1.0;
          ClearColor[2] = 1.0;
        }
        g_pD3DDeviceContext->ClearRenderTargetView( p_Window->p_RenderTargetView, ClearColor );
        p_Window->p_SwapChain->Present( 0, 0 );
    }
}

