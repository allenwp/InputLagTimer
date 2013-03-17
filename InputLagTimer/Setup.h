#pragma once
#include <vector>

class Setup
{
public:
  struct OutputSetting
  {
    void* output;
    double maxTimerResolution;
    unsigned int width;
    unsigned int height;
    DXGI_RATIONAL refreshRate;
  };

  struct AdapterSetting
  {
    void* adapter;
    std::vector<OutputSetting> outputSettings;
  };

  struct Settings
  {
    std::vector<AdapterSetting> adapterSettings;
  };

  Setup(void);
  virtual ~Setup(void);

  Settings pickSettings();

protected:
  void analizeSystem();
};

