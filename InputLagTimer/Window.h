#pragma once

class Window
{
public:
  Window(void);
  virtual ~Window(void);

  unsigned int windowNumber;
};

// TODO: Verify that this is being called when accessing elements from the multimap
bool operator<(const Window& lhs, const Window& rhs);
