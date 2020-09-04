//////////////////////////////////////////////////////
// Death Pit.cpp
//////////////////////////////////////////////////////

#include <windows.h>
#include <dxerr8.h>

#include "Constants.h"
#include "Engine.h"
#include "resource.h"

// Function prototypes
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void RegisterWindowClass(HINSTANCE hInstance);
void CreateAppWindow(HINSTANCE hInstance);
WPARAM StartMessageLoop();

// Global variables
HWND g_hWnd;
CEngine g_Engine;

//////////////////////////////////////////////////////
// WinMain()
//////////////////////////////////////////////////////
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
    RegisterWindowClass(hInstance);
    CreateAppWindow(hInstance);
    g_Engine.SetWindowHandle(g_hWnd);

	HRESULT hResult = g_Engine.GetCDirectSoundObj()->InitDirectSound();
    if (hResult == DS_OK)
	{
        hResult = g_Engine.GetCDirect3DObj()->InitD3D();
		if (hResult == D3D_OK)
		{
			ShowCursor(FALSE);
			ShowWindow(g_hWnd, SW_SHOWDEFAULT);
			UpdateWindow(g_hWnd);
			StartMessageLoop();
		}
		else
		{
        char* msg = g_Engine.GetCDirect3DObj()->GetErrorString();
		MessageBox(g_hWnd, msg, "Error", MB_OK);
        DXTRACE_ERR(msg, hResult);
		}
	}
	else
	{
		char* msg = g_Engine.GetCDirectSoundObj()->GetErrorString();
		MessageBox(g_hWnd, msg, "Error", MB_OK);
        DXTRACE_ERR(msg, hResult);
	}
    return 0;
}

//////////////////////////////////////////////////////
// WndProc()
//////////////////////////////////////////////////////
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		ValidateRect(g_hWnd, NULL);
		return 0;

	case WM_KEYDOWN:
		g_Engine.HandleKeys(wParam);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////
// RegisterWindowClass()
//////////////////////////////////////////////////////
void RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "DeathPit";
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	RegisterClassEx(&wc);
}

//////////////////////////////////////////////////////
// CreateAppWindow()
//////////////////////////////////////////////////////
void CreateAppWindow(HINSTANCE hInstance)
{
	// Calculate the proper size for the window given a client of 640x480
    DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
    DWORD dwWindowWidth   = WINDOW_WIDTH  + dwFrameWidth * 2;
    DWORD dwWindowHeight  = WINDOW_HEIGHT + dwFrameHeight * 2 + dwMenuHeight + dwCaptionHeight;

	g_hWnd = CreateWindowEx(
		NULL,
		"DeathPit",
		"Death Pit",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		dwWindowWidth,
		dwWindowHeight,
		GetDesktopWindow(),
		NULL,
		hInstance,
		NULL);
}

//////////////////////////////////////////////////////
// StartMessageLoop()
//////////////////////////////////////////////////////
WPARAM StartMessageLoop()
{
	MSG msg;
	while(1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Use idle time here
			g_Engine.ProcessGame();
		}
	}
	return msg.wParam;
}