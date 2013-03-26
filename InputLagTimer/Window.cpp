#include "stdafx.h"
#include "Window.h"


Window::Window(void)
{
}


Window::~Window(void)
{
}

bool operator<(const Window& lhs, const Window& rhs)
{
  return lhs.windowNumber < rhs.windowNumber;
}
