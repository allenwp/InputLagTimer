#include "Model.h"
#include <Windows.h>

Model::Model(void)
  : mFps(0)
{
  mValue.high = 0;
  mValue.low = 0;
}


Model::~Model(void)
{
}

void initialize()
{
  LARGE_INTEGER value;
  LARGE_INTEGER frequency;
  QueryPerformanceCounter(&value);
  QueryPerformanceFrequency(&frequency);

}

void Model::update()
{
  UINT16 old = mValue.low;
  incrementLow(&mValue);
  if(old > mValue.low)
  {
    incrementHigh(&mValue);
  }
}

Model::ValueType* Model::getValuePtr()
{
  return &mValue;
}

unsigned int* Model::getFpsPtr()
{
  return &mFps;
}

void Model::incrementLow(Model::ValueType* value)
{
  if(value->low == 99)
  {
    value->low = 0;
  }
  else
  {
    value->low++;
  }
}

void Model::incrementHigh(Model::ValueType* value)
{
  if(value->high == 999)
  {
    value->high = 0;
  }
  else
  {
    value->high++;
  }
}
