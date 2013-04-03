#pragma once
class Model
{
public:
  enum ErrorType
  {
    ERROR_COUNTER_OVERFLOW /* Program needs restart! */
  };

  Model(LARGE_INTEGER startingCount);
  virtual ~Model(void);

  void update();

protected:
  void reportError(ErrorType error, bool isPermanent);

  LARGE_INTEGER mStartingCount;
  LARGE_INTEGER mLastCount;
};

