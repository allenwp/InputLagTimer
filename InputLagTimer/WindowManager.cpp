#include "stdafx.h"
#include "WindowManager.h"

WindowManager::WindowManager(const Setup::Settings& settings)
{
  int deviceSortCount = 0;
  for(auto iter = settings.adapterSettings.begin(); iter != settings.adapterSettings.end(); ++iter)
  {
    Device device;
    ZeroMemory(&device, sizeof(Device));
    device.sortValue = deviceSortCount;

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
      D3D_DRIVER_TYPE_HARDWARE,
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
      referencedObj.insert(device.d3DDevice);
      referencedObj.insert(device.d3DDeviceConext);
      for(auto outputIter = iter->outputSettings.begin(); outputIter != iter->outputSettings.end(); ++outputIter)
      {
        // TODO: create the window and its swap chain, etc, etc.
        // then set it as the value for the current multimap
      }

      ++deviceSortCount;
    }
  }
}

WindowManager::~WindowManager(void)
{
  for(auto iter = referencedObj.begin(); iter != referencedObj.end(); ++iter)
  {
    (*iter)->Release();
  }
}

void WindowManager::render()
{

}

bool operator <(const WindowManager::Device& lhs, const WindowManager::Device& rhs)
{
    return lhs.sortValue < rhs.sortValue;
}
