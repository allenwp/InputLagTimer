#include "stdafx.h"
#include "Setup.h"

void Setup::getClosestDisplayModeToCurrent(IDXGIOutput* output, DXGI_MODE_DESC* outCurrentDisplayMode)
{
  DXGI_OUTPUT_DESC outputDesc;
  output->GetDesc(&outputDesc);
  HMONITOR hMonitor = outputDesc.Monitor;
  MONITORINFOEX monitorInfo;
  monitorInfo.cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo(hMonitor, &monitorInfo);
  DEVMODE devMode;
  devMode.dmSize = sizeof(DEVMODE);
  devMode.dmDriverExtra = 0;
  EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

  DXGI_MODE_DESC current;
  current.Width = devMode.dmPelsWidth;
  current.Height = devMode.dmPelsHeight;
  bool useDefaultRefreshRate = 1 == devMode.dmDisplayFrequency || 0 == devMode.dmDisplayFrequency;
  current.RefreshRate.Numerator = useDefaultRefreshRate ? 0 : devMode.dmDisplayFrequency;
  current.RefreshRate.Denominator = useDefaultRefreshRate ? 0 : 1;
  current.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  current.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
  current.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  output->FindClosestMatchingMode(&current, outCurrentDisplayMode, NULL);
}

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
      getClosestDisplayModeToCurrent(output, &outputSettings.bufferDesc);

      /* Find the desktop coordinates of the window: */
      DXGI_OUTPUT_DESC outputDesc;
      outputSettings.output->GetDesc(&outputDesc);
      outputSettings.windowPositionLeft = outputDesc.DesktopCoordinates.left;
      outputSettings.windowPositionTop = outputDesc.DesktopCoordinates.top;

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
