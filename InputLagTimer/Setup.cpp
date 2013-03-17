#include "stdafx.h"
#include "Setup.h"


Setup::Setup(void)
{
  analizeSystem();
}

Setup::~Setup(void)
{
}

const Setup::Settings& Setup::getSettings() const
{
  return mSettings;
}

void Setup::analizeSystem()
{

}
