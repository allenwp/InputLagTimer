#include "stdafx.h"
#include "Setup.h"

Setup::Setup(void)
{
  analizeSystem();
}

Setup::~Setup(void)
{
  for(auto iter = mSettings.adapterSettings.begin(); iter != mSettings.adapterSettings.end(); ++iter)
  {
    iter->adapter->Release();
    for(auto outIter = iter->outputSettings.begin(); outIter != iter->outputSettings.end(); ++outIter)
    {
      outIter->output->Release();
    }
  }
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

    bool foundAnOutput = false;
    UINT j = 0;
    IDXGIOutput* output;
    while(adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND)
    {
      OutputSetting outputSettings;
      ZeroMemory(&outputSettings, sizeof(OutputSetting));

      outputSettings.output = output;

      outputSettings.maxTimerResolution = 0.0;

      // TODO: Is the output surface the right place to get this info???
      //IDXGISurface* outputSurface = nullptr;
      //HRESULT result = output->GetDisplaySurfaceData(outputSurface);
      //DXGI_SURFACE_DESC* outputSurfaceDescription = nullptr;
      //outputSurface->GetDesc(outputSurfaceDescription);

      // TODO: figure out where to get these instead of hardcoding them...
      outputSettings.bufferDesc.Width = 1920; //outputSurfaceDescription->Width;
      outputSettings.bufferDesc.Height = 1080; //outputSurfaceDescription->Height;
      outputSettings.bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //outputSurfaceDescription->Format;
      outputSettings.bufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
      outputSettings.bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      // TODO: Don't hardcode the refresh rate: get it from the moinitor's current refresh rate
      outputSettings.bufferDesc.RefreshRate.Numerator = 0;
      outputSettings.bufferDesc.RefreshRate.Denominator = 1;

      adapterSettings.outputSettings.push_back(outputSettings);

      foundAnOutput = true;
      ++j;
    }

    if(foundAnOutput)
    {
      mSettings.adapterSettings.push_back(adapterSettings);
    }

    ++i;
  }
  factory->Release();
}
