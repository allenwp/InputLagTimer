#include "stdafx.h"
#include "WindowManager.h"
#include "Window.h"

WindowManager::WindowManager(const Setup::Settings& settings, HINSTANCE hInstance)
{
  Window::registerWindow(hInstance);

  for(auto iter = settings.adapterSettings.begin(); iter != settings.adapterSettings.end(); ++iter)
  {
    Device device;
    ZeroMemory(&device, sizeof(Device));

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
    
    HRESULT result = D3D11CreateDevice(iter->adapter,
      D3D_DRIVER_TYPE_UNKNOWN,
      NULL,
      D3D11_CREATE_DEVICE_SINGLETHREADED,
      featureLevels,
      numFeatureLevels,
      D3D11_SDK_VERSION,
      &device.d3DDevice,
      featureLevel,
      &device.d3DDeviceConext);
    if(S_OK == result)
    {
      mReferencedObj.insert(device.d3DDevice);
      mReferencedObj.insert(device.d3DDeviceConext);
      int numberOfViewports = iter->outputSettings.size();
      D3D11_VIEWPORT* viewports = new D3D11_VIEWPORT[numberOfViewports];
      int sizeOfViewports = sizeof(viewports) * numberOfViewports;
      ZeroMemory(viewports, sizeOfViewports);
      int i = 0;
      for(auto outputIter = iter->outputSettings.begin(); outputIter != iter->outputSettings.end(); ++outputIter)
      {
        Window* window = new Window(hInstance, *outputIter, device);
        DeviceWindowPair pair;
        pair.device = device;
        pair.window = window;
        mWindows.push_back(pair);

        /* Create the viewport for this window, needed for DXTK font rendering */
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        window->getSwapChain()->GetDesc(&swapChainDesc);
        viewports[i].Width = swapChainDesc.BufferDesc.Width;
        viewports[i].Height = swapChainDesc.BufferDesc.Height;
        viewports[i].MinDepth = 0.0f;
        viewports[i].MaxDepth = 1.0f;
        viewports[i].TopLeftX = 0;
        viewports[i].TopLeftY = 0;

        i++;
      }
      /* Setup the viewport */
      device.d3DDeviceConext->RSSetViewports( numberOfViewports, viewports );
      delete viewports;
    }
  }

  /* Go fullscreen after creating swap chains.
     http://msdn.microsoft.com/en-us/library/windows/desktop/ee417025(v=vs.85).aspx#multiple_monitors */
  for(auto iter = mWindows.begin(); iter != mWindows.end(); ++iter)
  {
    iter->window->setFullscreen(true);
  }

  LARGE_INTEGER startingCount;
  QueryPerformanceCounter(&startingCount);
  for(auto iter = mWindows.begin(); iter != mWindows.end(); ++iter)
  {
    iter->window->initializeModel(startingCount, iter->window->getSwapChain());
  }
}

WindowManager::~WindowManager(void)
{
  for(auto iter = mReferencedObj.begin(); iter != mReferencedObj.end(); ++iter)
  {
    (*iter)->Release();
  }

  for(auto iter = mWindows.begin(); iter != mWindows.end(); ++iter)
  {
    delete iter->window;
  }
}

void WindowManager::render()
{
  for(auto iter = mWindows.begin(); iter != mWindows.end(); ++iter)
  {
    iter->window->render(iter->device);
  }
}
