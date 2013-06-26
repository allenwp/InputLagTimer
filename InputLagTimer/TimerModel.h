#pragma once
class Model
{
public:
  struct TimerValue
  {
    unsigned int high;
    unsigned int low;
  };

  enum ErrorType
  {
    ERROR_TYPE_NONE,
    ERROR_TYPE_COUNTER_OVERFLOW, /* Program needs restart because the performance counter has no defined maximum and has wrapped around. */
    ERROR_TYPE_RENDER_TIME_VARIANCE_TOO_HIGH,
    ERROR_TYPE_MODEL_UPDATE_VARIANCE_TOO_HIGH
  };

  static void reportError(ErrorType error, bool isPermanent);

  static ErrorType getCurrentError();

  static int numColumns;

  Model(LARGE_INTEGER startingCount, IDXGISwapChain* swapChain);
  virtual ~Model(void);

  void update();
  /**
   * Should be called directly after the buffer is flipped.
   * This function will calculate the render time for the last frame
   */
  void renderComplete();

  /**
   * @return the timer value in ms.
   * This number will always be less than 1000 ms
   * This value has a precision of two decimal places.
   */
  TimerValue getTimerValue() const;

  /**
   * @return the column that the timer value should be drawn to: 0, 1, or 2
   */
  int getColumn() const;

  /**
   * @return The last render time in seconds.
   */
  double getLastRenderTime() const;

protected:
  static ErrorType mCurrerntError;
  static bool mIsCurrentErrorPermanent;

  LARGE_INTEGER mStartingCount;
  LARGE_INTEGER mLastCount;
  TimerValue mTimerValue;
  double mLastRenderTime;
  LARGE_INTEGER mCountsPerRefresh;
  LARGE_INTEGER mCountsSinceRefresh;
  DXGI_SWAP_CHAIN_DESC mSwapChainDesc;
  int mColumn;
};

