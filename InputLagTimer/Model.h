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
    ERROR_COUNTER_OVERFLOW, /* Program needs restart because the performance counter has no defined maximum and has wrapped around. */
    ERROR_RENDER_TIME_VARIANCE_TOO_HIGH
  };

  Model(LARGE_INTEGER startingCount);
  virtual ~Model(void);

  void update();
  void reportError(ErrorType error, bool isPermanent);

  /**
   * Returns the timer value in ms.
   * This number will always be less than 1000 ms
   * This value has a precision of two decimal places.
   */
  TimerValue getTimerValue() const;

protected:
  LARGE_INTEGER mStartingCount;
  LARGE_INTEGER mLastCount;
  TimerValue mTimerValue;
};

