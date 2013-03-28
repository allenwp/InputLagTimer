#pragma once

class Window
{
public:
  static ATOM registerWindow(HINSTANCE hInstance);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  static TCHAR windowClassName[];

  Window(void);
  virtual ~Window(void);

  unsigned int windowNumber;

protected:
  TCHAR* windowName;
};

// TODO: Verify that this is being called when accessing elements from the multimap
bool operator<(const Window& lhs, const Window& rhs);
