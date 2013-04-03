#include "stdafx.h"
#include "Model.h"


Model::Model(LARGE_INTEGER startingCount)
{
  mStartingCount = startingCount;
}


Model::~Model(void)
{
}

void Model::update()
{
  LARGE_INTEGER performanceFrequency;
  QueryPerformanceFrequency(&performanceFrequency);

  /* QUERY THE PERFORMANCE COUNTER!
     The remaining of this function must be consistent between
     models to ensure that the same latency to display occurs for
     each output. */
  LARGE_INTEGER currentCount;
  QueryPerformanceCounter(&currentCount);

  if(currentCount.QuadPart < mStartingCount.QuadPart)
  {
    reportError(ERROR_COUNTER_OVERFLOW, true);
  }

  LARGE_INTEGER countSinceStart;
  countSinceStart.QuadPart = currentCount.QuadPart - mStartingCount.QuadPart;

  double secondsSinceStart = ((double)countSinceStart.QuadPart) / performanceFrequency.QuadPart;

  ULONGLONG fullTimerValue = secondsSinceStart * 100000.0 + 0.5; /* Add 0.5 to round */
  ULONGLONG trimmedHigh = (fullTimerValue / 100000) * 100000;
  unsigned int timerValue = fullTimerValue - trimmedHigh;

  mLastCount = currentCount;
}

void Model::reportError(Model::ErrorType error, bool isPermanent)
{
  // TODO
}