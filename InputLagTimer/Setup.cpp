#include "stdafx.h"
#include "Setup.h"
#include <vector>

IDXGIFactory1* g_pDXGIFactory = NULL;
HINSTANCE g_hInstance;

void init(HINSTANCE hInst)
{
  g_hInstance = hInst;
  CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&g_pDXGIFactory) );
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
        int x = OutputDesc.DesktopCoordinates.left;
        int y = OutputDesc.DesktopCoordinates.top;
        int width = OutputDesc.DesktopCoordinates.right - x;
        int height = OutputDesc.DesktopCoordinates.bottom - y;

        WindowDataContainer *p_Window = new WindowDataContainer;

        p_Window->hWnd = CreateWindow( L"MY_SUPER_CLASS",
                                        L"MY_SUPER_NAME",
                                        WS_POPUP,
                                        x,
                                        y,
                                        width,
                                        height,
                                        NULL,
                                        0,
                                        g_hInstance,
                                        NULL );

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

ID3D10Device* g_pD3DDevice = NULL;
void CreateSwapChainsAndViews()
{
    for( int i = 0; i < WindowsArray.size(); i++ )
    {
        WindowDataContainer* p_Window = WindowsArray.at(i);

        // get the dxgi device
        IDXGIDevice* pDXGIDevice = NULL;
        g_pD3DDevice->QueryInterface( IID_IDXGIDevice, ( void** )&pDXGIDevice ); // COM stuff, hopefully you are familiar

        // create a swap chain
        DXGI_SWAP_CHAIN_DESC SwapChainDesc;

        // fill it in

        HRESULT hr = g_pDXGIFactory->CreateSwapChain( pDXGIDevice, &SwapChainDesc, &p_Window->p_SwapChain );
        pDXGIDevice->Release();
        pDXGIDevice = NULL;

         // get the backbuffer
        ID3D10Texture2D* p_BackBuffer = NULL;
        hr = p_Window->p_SwapChain->GetBuffer( 0, IID_ID3D10Texture2D, ( void** )&p_BackBuffer );

        // get the backbuffer desc
        D3D10_TEXTURE2D_DESC BackBufferDesc;
        p_BackBuffer->GetDesc( &BackBufferDesc );

        // create the render target view
        D3D10_RENDER_TARGET_VIEW_DESC RTVDesc;

        // fill it in

        g_pD3DDevice->CreateRenderTargetView( p_BackBuffer, &RTVDesc, &p_Window->p_RenderTargetView );
        p_BackBuffer->Release();
        p_BackBuffer = NULL;

        // Create depth stencil texture
        ID3D10Texture2D* p_DepthStencil = NULL;
        D3D10_TEXTURE2D_DESC descDepth;

        // fill it in


        g_pD3DDevice->CreateTexture2D( &descDepth, NULL, &p_DepthStencil );

        // Create the depth stencil view
        D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;

        // fill it in

        g_pD3DDevice->CreateDepthStencilView( p_DepthStencil, &descDSV, &p_Window->p_DepthStencilView );

    }

}

void MultiRender( )
{
    // Clear them all
    for( int i = 0; i < WindowsArray.size(); i++ )
    {
        WindowDataContainer* p_Window = WindowsArray.at(i);

        // There is the answer to your second question:
        g_pD3DDevice->OMSetRenderTargets( 1, &p_Window->p_RenderTargetView, p_Window->p_DepthStencilView );

        // Don't forget to adjust the viewport, in fullscreen it's not important...
        D3D10_VIEWPORT Viewport;
        Viewport.TopLeftX = 0;
        Viewport.TopLeftY = 0;
        Viewport.Width = p_Window->width;
        Viewport.Height = p_Window->height;
        Viewport.MinDepth = 0.0f;
        Viewport.MaxDepth = 1.0f;
        g_pD3DDevice->RSSetViewports( 1, &Viewport );

        // TO DO: AMAZING STUFF PER WINDOW
    }
}

