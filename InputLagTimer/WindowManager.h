#pragma once
#include "Setup.h"
#include "Window.h"
#include <map>

class WindowManager
{
public:
  struct Device
  {
    int sortValue;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceConext;
  };

  WindowManager(const Setup::Settings& settings);
  virtual ~WindowManager(void);

  void render();

protected:
  std::multimap<Device*, Window*> mWindowMap;
};

bool operator <(const WindowManager::Device& lhs, const WindowManager::Device& rhs);
