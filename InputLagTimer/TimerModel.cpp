#include "stdafx.h"
#include "TimerModel.h"

int Model::numColumns = 2;
Model::ErrorType Model::mCurrerntError = Model::ERROR_TYPE_NONE;
bool Model::mIsCurrentErrorPermanent = false;

double Model::mLastRenderTimeVariance = 0.0;
double Model::mMaxRenderTimeVariance = 0.0;
double Model::mDisplayRenderTimeVariance = 0.0;

int Model::mFrameCount = 0;
int Model::mFPS = 0;

double Model::mLowestAccuracy = 0.0;
double Model::mDisplayAccuracy = 0.0;

void Model::loopStarted(const std::vector<Model*>& models)
{
  /* Render time variance calculations and reporting */
  auto renderTimeIter = models.begin();
  double firstRenderTime = (*renderTimeIter)->mLastRenderTime;
  double highRenderTime = firstRenderTime;
  double lowRenderTime = firstRenderTime;
  ++renderTimeIter;
  while(renderTimeIter != models.end())
  {
    double renderTime = (*renderTimeIter)->mLastRenderTime;
    if(renderTime < lowRenderTime)
    {
      lowRenderTime = renderTime;
    }
    if(renderTime > highRenderTime)
    {
      highRenderTime = renderTime;
    }
    ++renderTimeIter;
  }
  mLastRenderTimeVariance = highRenderTime - lowRenderTime;

  if(mLastRenderTimeVariance > 0.0001)
  {
    Model::reportError(ERROR_TYPE_RENDER_TIME_VARIANCE_TOO_HIGH, false);
  }

  if(mLastRenderTimeVariance > mMaxRenderTimeVariance)
  {
    mMaxRenderTimeVariance = mLastRenderTimeVariance;
  }

  /* Accuracy reporting */
  auto accuracyIter = models.begin();
  double firstAccuracy = (*accuracyIter)->mLastAccuracy;
  double lowAccuracy = firstAccuracy;
  ++accuracyIter;
  while(accuracyIter != models.end())
  {
    double accuracy = (*accuracyIter)->mLastAccuracy;
    if(accuracy > lowAccuracy)
    {
      lowAccuracy = accuracy;
    }
    ++accuracyIter;
  }

  /* Add the render time variance to the lowest accuracy for total accuracy rating. */
  lowAccuracy += mLastRenderTimeVariance;

  if(lowAccuracy > 0.001)
  {
    reportError(ERROR_TYPE_ACCURACY_TOO_LOW, false);
  }

  if(lowAccuracy > mLowestAccuracy)
  {
    mLowestAccuracy = lowAccuracy;
  }

  recordRecordValuesForHUD();
  resetErrors();
}

void Model::loopComplete()
{
  ++mFrameCount;
}

void Model::reportError(Model::ErrorType error, bool isPermanent)
{
  if(!mIsCurrentErrorPermanent)
  {
    mIsCurrentErrorPermanent = isPermanent;
    mCurrerntError = error;
  }
}

Model::ErrorType Model::getCurrentError()
{
  return mCurrerntError;
}

double Model::getMaxRenderTimeVariance()
{
  return mMaxRenderTimeVariance;
}

int Model::getFPS()
{
  return mFPS;
}

void Model::recordRecordValuesForHUD()
{
  //TODO: If one second has passed
  if(true)
  {
    mFPS = mFrameCount;
    mFrameCount = 0;

    mDisplayRenderTimeVariance = mMaxRenderTimeVariance;
    mMaxRenderTimeVariance = 0.0;

    mDisplayAccuracy = mLowestAccuracy;
    mLowestAccuracy = 0.0;
  }
}

void Model::resetErrors()
{
  //TODO: 1.5 seconds after last non-permanent error...
  if(!mIsCurrentErrorPermanent && true)
  {
    mCurrerntError = ERROR_TYPE_NONE;
  }
}

Model::Model(LARGE_INTEGER startingCount, IDXGISwapChain* swapChain)
  :mStartingCount(startingCount),
  mLastCount(startingCount),
  mLastRenderTime(0.0),
  mColumn(0),
  mLastAccuracy(0.0)
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
    reportError(ERROR_TYPE_COUNTER_OVERFLOW, true);
  }

  LARGE_INTEGER countSinceStart, countSinceLast;
  countSinceStart.QuadPart = currentCount.QuadPart - mStartingCount.QuadPart;
  double secondsSinceStart = ((double)countSinceStart.QuadPart) / performanceFrequency.QuadPart;
  countSinceLast.QuadPart = currentCount.QuadPart - mLastCount.QuadPart;
  double secondsSinceLast = ((double)countSinceLast.QuadPart) / performanceFrequency.QuadPart;

  mLastAccuracy = secondsSinceLast;

  /* Timer Value */
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

Model::TimerValue Model::getTimerValue() const
{
  return mTimerValue;
}

int Model::getColumn() const
{
  return mColumn;
}

