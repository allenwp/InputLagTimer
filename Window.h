#pragma once

#include "Resource.h"

class Window
{
public:
  static LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static ATOM RegisterWndClass(HINSTANCE hInstance);
  static ATOM MyRegisterClass(HINSTANCE hInstance);
  static HANDLE CreateThread(HINSTANCE hInstance);

  Window(HINSTANCE hInstance);
  virtual ~Window(void);

  void MessageLoop();

private:
  static DWORD WINAPI StartWindowThread( __in  LPVOID lpParameter );

  HINSTANCE mHInstance;
  HWND mHWnd;
};

