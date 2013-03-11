#pragma once

#include "Resource.h"
#include "Model.h"

class Window
{
public:
	static ATOM RegisterWndClass(HINSTANCE hInstance);
  static HANDLE CreateThread(HINSTANCE hInstance);
  Window(HINSTANCE hInstance);
  virtual ~Window(void);

private:
  static DWORD WINAPI StartWindowThread( __in  LPVOID lpParameter );
  static LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  void MessageLoop();

  HINSTANCE mHInstance;
  HWND mHWnd;

  Model mModel;
};

