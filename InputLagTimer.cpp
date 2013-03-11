// InputLagTimer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "InputLagTimer.h"
#include "Window.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!Window::RegisterWndClass(hInstance))
	{
		MessageBox(NULL, _T("Cannot register window class"), _T("Error"), MB_OK);
		return -1;
	}

	HANDLE threads[2];

	for (int i = 0; i < _countof(threads); i++)
  {
		threads[i] = Window::CreateThread(hInstance);
  }

	WaitForMultipleObjects(_countof(threads), threads, TRUE, INFINITE);

	return 0;
}
