// InputLagTimer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "InputLagTimer.h"
#include "Window.h"
#include <vector>

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
  std::vector<Window::SwapChainTarget> targets;
  
  IDXGIFactory1 * pFactory;
  HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory) );
  if(SUCCEEDED(hr))
  {
    UINT i = 0;
    IDXGIAdapter1 * pAdapter;
    while(SUCCEEDED(pFactory->EnumAdapters1(i, &pAdapter)))
    {
      UINT j = 0;
      IDXGIOutput * pOutput;
      while(pAdapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND)
      {
        if(targets.size() < 2)
        {
          Window::SwapChainTarget target;
          pAdapter->AddRef();
          pOutput->AddRef();
          target.adapter = pAdapter;
          target.output = pOutput;
          targets.push_back(target);
        }
        ++j;
      }
      ++i;
    }
  }
  if(pFactory)
  {
    pFactory->Release();
  }

	for (int i = 0; i < _countof(threads); i++)
  {
    Window::ThreadArgs* args = new Window::ThreadArgs();
    args->hInstance = hInstance;
    if(targets.size() > i)
    {
      args->target = targets[i];
    }
    else
    {
      args->target.adapter = nullptr;
      args->target.output = nullptr;
    }
    threads[i] = Window::CreateThread(args);
  }

	WaitForMultipleObjects(_countof(threads), threads, TRUE, INFINITE);

	return 0;
}
