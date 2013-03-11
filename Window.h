#pragma once

#include "Resource.h"

class Window
{
public:
  static HANDLE CreateThread(HINSTANCE hInstance);
  Window(HINSTANCE hInstance);
  virtual ~Window(void);

private:
  static DWORD WINAPI StartWindowThread( __in  LPVOID lpParameter );
	static ATOM RegisterWndClass(HINSTANCE hInstance);
  static ATOM MyRegisterClass(HINSTANCE hInstance);
  static LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  void MessageLoop();

  HINSTANCE mHInstance;
  HWND mHWnd;
};

