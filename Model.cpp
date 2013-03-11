#include "stdafx.h"
#include "Model.h"

Model::Model(void)
  : mFps(0)
{
  mValue.value.high = 0;
  mValue.value.low = 0;

  LARGE_INTEGER value;
  LARGE_INTEGER frequency;
  QueryPerformanceCounter(&value);
  QueryPerformanceFrequency(&frequency);
}


Model::~Model(void)
{
}

void Model::update()
{
  UINT16 old = mValue.value.low;
  incrementLow(&mValue);
  if(old > mValue.value.low)
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
  if(value->value.low == 99)
  {
    value->value.low = 0;
  }
  else
  {
    value->value.low++;
  }
}

void Model::incrementHigh(Model::ValueType* value)
{
  if(value->value.high == 999)
  {
    value->value.high = 0;
  }
  else
  {
    value->value.high++;
  }
}
