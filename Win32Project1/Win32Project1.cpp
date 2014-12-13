#include "stdafx.h"
#include "Win32Project1.h"

#define MAX_LOADSTRING 100
#define WIDTH_OF_FIXED_PICTURE 500
#define HEIGHT_OF_FIXED_PICTURE 500

HINSTANCE hInst;					
TCHAR szTitle[MAX_LOADSTRING];		
TCHAR szWindowClass[MAX_LOADSTRING];

HDC screenHandleDeviceContext;
HDC fixedHandleDeviceContext;
HDC virtualHandleDeviceContext;
bool isDrawingByPencil;
POINT cursorPosition;
POINT previousCursorPosition;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void ToProcessClickingByLeftMouseButton(HWND, UINT, WPARAM, LPARAM);
void ToProcessReleaseByLeftMouseButton(HWND, UINT, WPARAM, LPARAM);
void ToProcessMovementByMouse(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;
	HBITMAP hBitmap;
	LPSIZE lpDimension;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	fixedHandleDeviceContext = CreateCompatibleDC(screenHandleDeviceContext);
	if (fixedHandleDeviceContext != NULL)
	{
		hBitmap = CreateCompatibleBitmap(screenHandleDeviceContext, WIDTH_OF_FIXED_PICTURE, HEIGHT_OF_FIXED_PICTURE); //from where to take the original size?
		if (hBitmap != NULL)
			SelectObject(fixedHandleDeviceContext, hBitmap);
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32PROJECT1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   screenHandleDeviceContext = GetDC(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT r;
	HBRUSH hBrush;
	HPEN hPen;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//- перенести изображение из fixedHandleDeviceContext в screenHandleDeviceContext (с установленным масштабом и позицией)
		//- отрисовать как-то screenHandleDeviceContext
		BitBlt(hdc, 0, 0, WIDTH_OF_FIXED_PICTURE, HEIGHT_OF_FIXED_PICTURE, fixedHandleDeviceContext, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DeleteDC(fixedHandleDeviceContext);
		ReleaseDC(hWnd, screenHandleDeviceContext);
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		ToProcessClickingByLeftMouseButton(hWnd, message, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		ToProcessReleaseByLeftMouseButton(hWnd, message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		ToProcessMovementByMouse(hWnd, message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void ToProcessClickingByLeftMouseButton(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	cursorPosition.x = GET_X_LPARAM(lParam);
	cursorPosition.y = GET_Y_LPARAM(lParam);
	isDrawingByPencil = true;
	//create virtualDC
}

void ToProcessReleaseByLeftMouseButton(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	isDrawingByPencil = false;
	DeleteDC(virtualHandleDeviceContext);
}

void ToProcessMovementByMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	previousCursorPosition.x = cursorPosition.x;
	previousCursorPosition.y = cursorPosition.y;
	
	//нарисовать линию от previousCursorPosition до cursorPosition

	//занесли в текущий(3ий) hdc рисунок из 2ого;
	//нарисовали в текущий(3ий) объект(линию).
	//Затем в этом же событии вы должны вызвать перерисовку окна

	//move virtual to fixedDc
}
