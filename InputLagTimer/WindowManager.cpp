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

    //TODO: create the device and device context, assign it and retain it, release it in ~

    for(auto outputIter = iter->outputSettings.begin(); outputIter != iter->outputSettings.end(); ++outputIter)
    {
      // TODO: create the window and its swap chain, etc, etc.
      // then set it as the value for the current multimap
    }

    ++deviceSortCount;
  }
}

WindowManager::~WindowManager(void)
{
}

void WindowManager::render()
{

}

bool operator <(const WindowManager::Device& lhs, const WindowManager::Device& rhs)
{
    return lhs.sortValue < rhs.sortValue;
}
