#include "stdafx.h"
#include "TimerModel.h"

int Model::numColumns = 2;

Model::Model(LARGE_INTEGER startingCount, IDXGISwapChain* swapChain)
  :mStartingCount(startingCount),
  mLastCount(startingCount),
  mLastRenderTime(0.0),
  mColumn(0)
{
  mCountsSinceRefresh.QuadPart = 0;

  ZeroMemory(&mSwapChainDesc, sizeof(mSwapChainDesc));
  swapChain->GetDesc(&mSwapChainDesc);

  /* Refresh rate calculations */
  LARGE_INTEGER performanceFrequency;
  QueryPerformanceFrequency(&performanceFrequency);
  mCountsPerRefresh.QuadPart = performanceFrequency.QuadPart * mSwapChainDesc.BufferDesc.RefreshRate.Denominator / mSwapChainDesc.BufferDesc.RefreshRate.Numerator;
}


Model::~Model(void)
{
}

union uFloat
{
  float value;
  unsigned __int32 iValue;
};

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

  LARGE_INTEGER countSinceStart, countSinceLast;
  countSinceStart.QuadPart = currentCount.QuadPart - mStartingCount.QuadPart;
  countSinceLast.QuadPart = currentCount.QuadPart - mLastCount.QuadPart;

  /* Timer Value */
  double secondsSinceStart = ((double)countSinceStart.QuadPart) / performanceFrequency.QuadPart;
  ULONGLONG fullTimerValue = secondsSinceStart * 100000.0 + 0.5; /* Add 0.5 to round */
  ULONGLONG trimmedHigh = (fullTimerValue / 100000) * 100000;
  unsigned int iTimerValue = fullTimerValue - trimmedHigh;
  mTimerValue.high = iTimerValue / 100; /* Convert to ms */
  mTimerValue.low = iTimerValue - (mTimerValue.high * 100); /* Sub-milliseconds */

  /* Column & once-per-refresh stuff */
  mCountsSinceRefresh.QuadPart += countSinceLast.QuadPart;
  if(mCountsSinceRefresh.QuadPart >= mCountsPerRefresh.QuadPart)
  {
    mCountsSinceRefresh.QuadPart -= mCountsPerRefresh.QuadPart;
    mColumn++;
    if(mColumn > numColumns - 1)
    {
      mColumn = 0;
    }
  }

  mLastCount = currentCount;
}

void Model::renderComplete()
{
  LARGE_INTEGER currentCount;
  QueryPerformanceCounter(&currentCount);

  LARGE_INTEGER renderCount;
  renderCount.QuadPart = currentCount.QuadPart - mLastCount.QuadPart;

  LARGE_INTEGER performanceFrequency;
  QueryPerformanceFrequency(&performanceFrequency);

  mLastRenderTime = ((double)renderCount.QuadPart) / performanceFrequency.QuadPart;
}

void Model::reportError(Model::ErrorType error, bool isPermanent)
{
  // TODO
}

Model::TimerValue Model::getTimerValue() const
{
  return mTimerValue;
}

int Model::getColumn() const
{
  return mColumn;
}

double Model::getLastRenderTime() const
{
  return mLastRenderTime;
}