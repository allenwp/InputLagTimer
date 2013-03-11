#include "stdafx.h"
#include "Window.h"
#include <assert.h>

#define MAX_LOADSTRING 100
bool windowClassRegistered = false;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

ATOM Window::RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

  // Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_INPUTLAGTIMER, szWindowClass, MAX_LOADSTRING);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= Window::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INPUTLAGTIMER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; // MAKEINTRESOURCE(IDC_INPUTLAGTIMER)
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  windowClassRegistered = true;

	return RegisterClassEx(&wcex);
}

HANDLE Window::CreateThread(HINSTANCE hInstance)
{
	return ::CreateThread(NULL, 0, Window::StartWindowThread, hInstance, 9, NULL);
}

//
//   FUNCTION: (formerly) InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
Window::Window(HINSTANCE hInstance)
{
  /* Window::RegisterWndClass(HINSTANCE hInstance) must be called first! */
  assert(windowClassRegistered);

  mHInstance = hInstance;

  mHWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   ShowWindow(mHWnd, SW_SHOWDEFAULT);
   UpdateWindow(mHWnd);
}

Window::~Window(void)
{
}

DWORD WINAPI Window::StartWindowThread( __in  LPVOID lpParameter )
{
	Window window((HINSTANCE)lpParameter);

	window.MessageLoop();

	return 0;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the window.
//
//  WM_PAINT	- Paint the window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
    // TODO: The multitheaded GUI didn't handle this message type... Figure out why...
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Window::MessageLoop()
{
	MSG msg;

  // Doing it with accelerators... Might want to bring this back at some point...
  //HACCEL hAccelTable;
	//hAccelTable = LoadAccelerators(mHInstance, MAKEINTRESOURCE(IDC_INPUTLAGTIMER));
	//// Main message loop:
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}
  
	while( true )
	{
		while (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		if (msg.message == WM_QUIT)
			break;

    // temporary for testing
    mModel.update();
		// TODO: Render(); or something

		// Give other windows some time to process messages
    Sleep(1);
		// TODO: usleep(100ULL);
	}
}
