#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "uxtheme")

#include <windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <string>
#include "basic.h"

TCHAR szClassName[] = TEXT("Window");
WNDPROC DefaultEditWndProc;

struct DATA {
	HWND hWnd;
	CBasic * basic;
};

DWORD WINAPI ThreadFunc(LPVOID lpV)
{
	DATA * data = (DATA*)lpV;
	data->basic->Run();
	PostMessage(data->hWnd, WM_APP, 0, 0);
	return 0;
}

void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::wstring::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

LRESULT CALLBACK EditProc1(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		if (wParam == VK_RETURN)
		{
			int nLineIndex = (int)SendMessage(hWnd, EM_LINEFROMCHAR, -1, 0);
			TCHAR szText[10];
			szText[0] = _countof(szText);
			SendMessage(hWnd, EM_GETLINE, nLineIndex, (LPARAM)szText);
			nLineIndex = _wtol(szText);
			LRESULT lresult = CallWindowProc(DefaultEditWndProc, hWnd, msg, wParam, lParam);
			wsprintf(szText, TEXT("%d "), (nLineIndex / 10) * 10 + 10);
			SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)szText);
			return lresult;
		}
		break;
	case WM_KEYUP:
		{
			LRESULT lresult = CallWindowProc(DefaultEditWndProc, hWnd, msg, wParam, lParam);
			if (GetWindowTextLength(hWnd) == 0)
			{
				SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)TEXT("10 "));
			}
			return lresult;
		}
		break;
	case WM_PASTE:
		{
			OpenClipboard(NULL);
			HANDLE hText = GetClipboardData(CF_UNICODETEXT);
			if (hText)
			{
				LPWSTR lpszBuf = (LPWSTR)GlobalLock(hText);
				{
					std::wstring strClipboardText(lpszBuf);
					ReplaceAll(strClipboardText, L"\r\n", L"\n");
					ReplaceAll(strClipboardText, L"\r", L"\n");
					ReplaceAll(strClipboardText, L"\n", L"\r\n");
					SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)strClipboardText.c_str());
				}
				GlobalUnlock(hText);
			}
			CloseClipboard();
		}
		return 0;
	default:
		break;
	}
	return CallWindowProc(DefaultEditWndProc, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton1;
	static HWND hButton2;
	static HWND hEdit1;
	static HWND hEdit2;
	static DOUBLE dControlHeight = 32.0;
	static HFONT hFont;
	static CBasic * basic;
	static DATA data;
	static HANDLE hThread;
	switch (msg)
	{
	case WM_CREATE:
		{
			HTHEME hTheme = OpenThemeData(hWnd, VSCLASS_AEROWIZARD);
			LOGFONT lf = { 0 };
			GetThemeFont(hTheme, NULL, AW_HEADERAREA, 0, TMT_FONT, &lf);
			hFont = CreateFontIndirectW(&lf);
			dControlHeight = lf.lfHeight * 1.8;
			if (dControlHeight < 0.0) dControlHeight = -dControlHeight;
			CloseThemeData(hTheme);
		}
		hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("実行(F5)"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hButton1, WM_SETFONT, (WPARAM)hFont, 0);
		hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("停止(Esc)"), WS_VISIBLE | WS_CHILD | WS_DISABLED, 0, 0, 0, 0, hWnd, (HMENU)IDCANCEL, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hButton2, WM_SETFONT, (WPARAM)hFont, 0);
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		DefaultEditWndProc = (WNDPROC)SetWindowLongPtr(hEdit1, GWLP_WNDPROC, (LONG_PTR)EditProc1);
		SendMessage(hEdit1, EM_LIMITTEXT, 0, 0);
		SendMessage(hEdit1, WM_SETFONT, (WPARAM)hFont, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hEdit2, EM_LIMITTEXT, 0, 0);
		SendMessage(hEdit2, WM_SETFONT, (WPARAM)hFont, 0);
		basic = new CBasic(hEdit1, hEdit2);
		SendMessage(hEdit1, EM_REPLACESEL, 0, (LPARAM)TEXT("10 "));
		SetFocus(hEdit1);
		break;
	case WM_SIZE:
		MoveWindow(hButton1, 10, 10, 256, (int)dControlHeight, TRUE);
		MoveWindow(hButton2, 276, 10, 256, (int)dControlHeight, TRUE);
		MoveWindow(hEdit1, 10, (int)(dControlHeight + 20), (LOWORD(lParam) - 30) / 2, (int)(HIWORD(lParam) - dControlHeight - 30), TRUE);
		MoveWindow(hEdit2, (LOWORD(lParam) - 30) / 2 + 20, (int)(dControlHeight + 20), (LOWORD(lParam) - 30) / 2, (int)(HIWORD(lParam) - dControlHeight - 30), TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EnableWindow(hButton1, FALSE);
			EnableWindow(hButton2, TRUE);
			EnableWindow(hEdit1, FALSE);
			EnableWindow(hEdit2, FALSE);
			{
				HMENU hMenu = GetSystemMenu(hWnd, FALSE);
				EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
			}
			data.basic = basic;
			data.hWnd = hWnd;
			DWORD d;
			hThread = CreateThread(NULL, 0, ThreadFunc, (LPVOID)&data, 0, &d);
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			basic->abort();
		}
		else if (LOWORD(wParam) == 1001)
		{
			HWND hFocus = GetFocus();
			if (hFocus == hEdit1 || hFocus == hEdit2)
			{
				SendMessage(hFocus, EM_SETSEL, 0, -1);
			}
		}
		break;
	case WM_SETFOCUS:
		SetFocus(hEdit1);
		break;
	case WM_APP:
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		hThread = 0;
		EnableWindow(hButton1, TRUE);
		EnableWindow(hButton2, FALSE);
		EnableWindow(hEdit1, TRUE);
		EnableWindow(hEdit2, TRUE);
		GetSystemMenu(hWnd, TRUE);
		SetFocus(hEdit1);
		break;
	case WM_DESTROY:
		delete basic;
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("TinyBasic GUI"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	ACCEL Accel[] = { { FVIRTKEY,VK_F5,IDOK },{ FVIRTKEY,VK_ESCAPE,IDCANCEL },{ FVIRTKEY | FCONTROL,'A',1001 } };
	HACCEL hAccel = CreateAcceleratorTable(Accel, _countof(Accel));
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	DestroyAcceleratorTable(hAccel);
	return (int)msg.wParam;
}
