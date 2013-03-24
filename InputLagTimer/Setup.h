#pragma once
#include <vector>

class Setup
{
public:
  struct OutputSetting
  {
    IDXGIOutput* output;
    double maxTimerResolution;
    DXGI_MODE_DESC bufferDesc;
  };

  struct AdapterSetting
  {
    IDXGIAdapter* adapter;
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

