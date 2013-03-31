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
    LONG windowPositionTop;
    LONG windowPositionLeft;
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

  static void getClosestDisplayModeToCurrent(IDXGIOutput* output, DXGI_MODE_DESC* outCurrentDisplayMode);

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

