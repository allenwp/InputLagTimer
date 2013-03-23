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

  UINT i = 0;
  IDXGIAdapter* adapter;
  while(factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) 
  {
    AdapterSetting adapterSettings;
    adapterSettings.adapter = adapter;
    adapter->AddRef();

    UINT j = 0;
    IDXGIOutput* output;
    while(adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND)
    {
      OutputSetting outputSettings;
      ZeroMemory(&outputSettings, sizeof(OutputSetting));

      outputSettings.output = output;
      output->AddRef();

      outputSettings.maxTimerResolution = 0.0;

      // TODO: Is the output surface the right place to get this info???
      IDXGISurface* outputSurface;
      output->GetDisplaySurfaceData(outputSurface);
      DXGI_SURFACE_DESC* outputSurfaceDescription;
      outputSurface->GetDesc(outputSurfaceDescription);
      outputSettings.bufferDesc.Width = outputSurfaceDescription->Width;
      outputSettings.bufferDesc.Height = outputSurfaceDescription->Height;
      outputSettings.bufferDesc.Format = outputSurfaceDescription->Format;
      outputSettings.bufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
      outputSettings.bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      // TODO: Don't hardcode the refresh rate: get it from the moinitor's current refresh rate
      outputSettings.bufferDesc.RefreshRate.Numerator = 1;
      outputSettings.bufferDesc.RefreshRate.Denominator = 60;

      adapterSettings.outputSettings.push_back(outputSettings);
    }

    mSettings.adapterSettings.push_back(adapterSettings);

    ++i;
  } 
}
