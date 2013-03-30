#pragma once
#include "Setup.h"
#include <map>
#include <unordered_set>

class Window;

class WindowManager
{
public:
  struct Device
  {
    ID3D11Device* d3DDevice;
    ID3D11DeviceContext* d3DDeviceConext;
  };

  struct DeviceWindowPair
  {
    Device device;
    Window* window;
  };

  WindowManager(const Setup::Settings& settings, HINSTANCE hInstance);
  virtual ~WindowManager(void);

  void render();

protected:
  std::vector<DeviceWindowPair> mWindows;
  std::unordered_set<IUnknown*> mReferencedObj;
};
