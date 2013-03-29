#pragma once
#include "Setup.h"
#include "Window.h"
#include <map>
#include <unordered_set>

class WindowManager
{
public:
  struct Device
  {
    int sortValue;
    ID3D11Device* d3DDevice;
    ID3D11DeviceContext* d3DDeviceConext;
  };

  WindowManager(const Setup::Settings& settings, HINSTANCE hInstance);
  virtual ~WindowManager(void);

  void render();

protected:
  std::multimap<Device, Window*> mWindowMap;
  std::unordered_set<IUnknown*> mReferencedObj;
};

// TODO: Verify that this is being called when accessing elements from the multimap
bool operator <(const WindowManager::Device& lhs, const WindowManager::Device& rhs);
