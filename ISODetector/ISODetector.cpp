#include "stdafx.h"
#include "ISODetector.h"
#include "isod.h"

HINSTANCE hInst;
MSG msg;
OPENFILENAME ofn;
Isod detector;
HWND outField1;
HWND outField2;
HWND outField3;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Contact(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Help(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	hInst = hInstance;
	// -----------------------Создание класса окна--------------------------------
	WNDCLASSEXW wcex;
	wchar_t szTitle[] = L"ISODetector";
	wchar_t szWindowClass[] = L"MainWindow";

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ISODETECTOR));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ISODETECTOR);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
		return FALSE;
	}
	
	//-------------------------Создание окна------------------------------------------------
	HWND hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME, szWindowClass, szTitle, WS_OVERLAPPED | WS_BORDER | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		MessageBox(NULL, L"Cannot create main window", L"Error", MB_OK);
		return FALSE;
	}

	//-------------------------Кнопка Загрузки--------------------------------------------------
	HWND ButtonLoad = CreateWindow(L"button", L"Загрузить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 20, 100, 25, hWnd, (HMENU)ID_BUTTON_LOAD, hInstance, NULL);
	if (!ButtonLoad)
	{
		MessageBox(NULL, L"Cannot create ButtonLoad", L"Error", MB_OK);
		return FALSE;
	}

	//-------------------------Кнопка Распознать--------------------------------------------------
	HWND ButtonDetect = CreateWindow(L"button", L"Распознать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 65, 100, 25, hWnd, (HMENU)ID_BUTTON_DETECT, hInstance, NULL);
	if (!ButtonDetect)
	{
		MessageBox(NULL, L"Cannot create ButtonDetect", L"Error", MB_OK);
		return FALSE;
	}
	HWND info1 = CreateWindow(L"static", L"Код владельца", WS_EX_CLIENTEDGE | WS_CHILD | WS_VISIBLE, 10, 110, 105, 25, hWnd, NULL, hInstance, 0);
	outField1 = CreateWindow(L"Edit", NULL, WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 140, 105, 25, hWnd, NULL, hInstance, 0);
	HWND info2 = CreateWindow(L"static", L"Регистр. номер", WS_EX_CLIENTEDGE | WS_CHILD | WS_VISIBLE, 10, 170, 105, 25, hWnd, NULL, hInstance, 0);
	outField2 = CreateWindow(L"Edit", NULL, WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 200, 105, 25, hWnd, NULL, hInstance, 0);
	HWND info3 = CreateWindow(L"static", L"Типоразмер", WS_EX_CLIENTEDGE | WS_CHILD | WS_VISIBLE, 10, 240, 105, 25, hWnd, NULL, hInstance, 0);
	outField3 = CreateWindow(L"Edit", NULL, WS_EX_CLIENTEDGE | WS_BORDER | WS_CHILD | WS_VISIBLE, 10, 270, 105, 25, hWnd, NULL, hInstance, 0);
	// -----------------------Инициализация структуры OPENFILENAME--------------------------------
	wchar_t ofnFilter[] = L"Bitmap files(*.bmp)\0*.bmp\0";
	wchar_t szFile[MAX_BUFFER_SIZE];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = ofnFilter;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//-----------------------Показ окна-------------------------------------------------------------
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


BOOL DetectAndRecolorBitmap(wchar_t* fileName, BITMAP bitmap)
{
	if (!detector.LoadBitmap(fileName, bitmap)) {
		return FALSE;
	}
	if (!detector.GetDetect()) {
		return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BITMAP bitmap;
	static BOOL fileOpenFlag = 0;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
			case ID_BUTTON_LOAD:
				if (!GetOpenFileName(&ofn)) {
					MessageBox(NULL, L"Cannot load file", L"Error", MB_OK);
					return FALSE;
				}
				fileOpenFlag = 1;
				InvalidateRect(hWnd, NULL, NULL);
				break;
			case ID_BUTTON_DETECT:
				if (!fileOpenFlag) {
					MessageBox(NULL, L"Нечего распознавать", L"Error", MB_OK);
					break;
				}
				if (DetectAndRecolorBitmap(ofn.lpstrFile, bitmap)) {
					SetWindowText(outField1, detector.ownerCode);
					SetWindowText(outField2, detector.number);
					SetWindowText(outField3, detector.typeCode);
				}				
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_CONTACT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_CONTACTBOX), hWnd, Contact);
				break;
			case IDM_HELP:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPBOX), hWnd, Help);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (fileOpenFlag) {
			RECT Rect;
			HANDLE hBitmap = LoadImage(hInst, ofn.lpstrFile, IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			if (!hBitmap) {
				MessageBox(NULL, L"Cannot load Bitmap", L"Error", MB_OK);
				break;
			}
			GetObject(hBitmap, sizeof(BITMAP), &bitmap);
			HDC hCompatibleDC = CreateCompatibleDC(hdc);
			HANDLE hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
			GetClientRect(hWnd, &Rect);
			StretchBlt(hdc, 120, 0, Rect.right -120, Rect.bottom, hCompatibleDC, 0, 0, bitmap.bmWidth,
				bitmap.bmHeight, SRCCOPY);
			SelectObject(hCompatibleDC, hOldBitmap);
			ReleaseDC(hWnd, hdc);
			ReleaseDC(hWnd, hCompatibleDC);
			DeleteObject(hBitmap);
			DeleteDC(hCompatibleDC);
			break;
		}
		EndPaint(hWnd, &ps);
		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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

// Обработчик сообщений для окна "Связь с разработчиком".
INT_PTR CALLBACK Contact(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

// Обработчик сообщений для окна "Помощь".
INT_PTR CALLBACK Help(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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