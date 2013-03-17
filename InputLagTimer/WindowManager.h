#pragma once
#include "Setup.h"

class WindowManager
{
public:
  WindowManager(const Setup::Settings& settings);
  virtual ~WindowManager(void);

  void render();
};

