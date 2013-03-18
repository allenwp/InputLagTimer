#include "stdafx.h"
#include "Setup.h"
#include <d3d11.h>

Setup::Setup(void)
{
  analizeSystem();
}

Setup::~Setup(void)
{
}

const Setup::Settings& Setup::getSettings() const
{
  return mSettings;
}

void Setup::analizeSystem()
{
  IDXGIFactory* factory = NULL;
  CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&factory));


}
