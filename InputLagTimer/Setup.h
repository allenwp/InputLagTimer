#pragma once
#include <vector>
#include <dxgitype.h>

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

  const Settings& getSettings() const;

protected:
  /**
   * Analyses the system using a DXGI factory to enumerate adapters
   * and outputs. Chooses settings and fills in the mSettings structure.
   */
  void analizeSystem();

  Settings mSettings;
};

