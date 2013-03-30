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
      for(auto outputIter = iter->outputSettings.begin(); outputIter != iter->outputSettings.end(); ++outputIter)
      {
        Window* window = new Window(hInstance, *outputIter, device);
        DeviceWindowPair pair;
        pair.device = device;
        pair.window = window;
        mWindows.push_back(pair);
      }
    }
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
