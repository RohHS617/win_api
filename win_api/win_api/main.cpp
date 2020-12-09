#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <atlstr.h>
#include "resource.h"
#define IDC_SEND 1003 
#define IDC_EDIT 100 

HINSTANCE hInst;
static bool flag;
static bool offset_flag;
using namespace std;
HWND hwndChild[100];
TCHAR Winbuffer[100][1000];

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam);

CString ConvertToHex(HWND hwnd, unsigned char* data, int size)
{
	CString returnvalue;
	CString str;
	CString temporary3;
	int offset=0;
	for (int x = 0;x < size; x++)
	{
		CString temporary;
		CString temporary2;
		
		if (((x % 16) == 0) && (x != 0))
		{
			returnvalue += _T("                 ");
			returnvalue += str;
			str = "";
			returnvalue += _T("\r\n");
			offset += 16;
			if(!offset_flag)
				temporary3.Format(_T("%07X\t\t"), offset);
			else
				temporary3.Format(_T("%07d\t\t"), offset);
			returnvalue += temporary3;
		}
		else if (x == 0) {
			if (!offset_flag)
				temporary3.Format(_T("%07X\t\t"), offset);
			else
				temporary3.Format(_T("%07d\t\t"), offset);
			returnvalue += temporary3;
		}
		int value = (int)(data[x]);
		char c = (char)(data[x]);
		temporary.Format(_T("%02X\t"), value);
		if (c == ' ')
			c = '.';
		else if (c == '\n')
			c = '.';
		temporary2.Format(_T("%c"),c);
		returnvalue += temporary;
		str += temporary2;
	}
	
	return returnvalue;
}


void FileRead(HWND hwnd, LPCTSTR filename)
{
	HANDLE hFile;
	unsigned char *buffer;
	DWORD size, CharNum;
	int fileSize;
	unsigned int nHexValue;

	hFile = CreateFile(filename, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	fileSize = GetFileSize(hFile, &size);
	buffer = (unsigned char*)malloc(fileSize);
	memset(buffer, 0, fileSize);
	ReadFile(hFile, buffer, fileSize, &CharNum, NULL);
	//buffer[(int)fileSize / sizeof(char)] = NULL;
	CString temp;
	CString buffer_cs;
	
	buffer_cs = buffer;
	temp = ConvertToHex(hwnd, buffer,fileSize);
	SetWindowText(hwnd, temp);
	free(buffer);
	CloseHandle(hFile);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{

	HWND hwnd;
	MSG	msg;
	WNDCLASSEX WndClass;
	HACCEL hAcc;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_IBEAM);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = _T("Windows Class Name");
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hwnd = CreateWindow(_T("Windows Class Name"),
		_T("Hex Viewer"),
		WS_OVERLAPPEDWINDOW,
		200,
		200,
		1600,
		650,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	static HWND hIndex;
	static HWND button;

	RECT rt;
	OPENFILENAME OFN;
	TCHAR str[100];
	static TCHAR lpstrFile[100] = _T("");
	static TCHAR filepath[1000], folder[100], filename[10][100];
	static int count;
	TCHAR* pStr;

	HDC hdc;
	PAINTSTRUCT lp;
	CString index=_T("Offset    Hex            00            01            02            03            04            05            06            07            08            09            0A            0B           0C            0D            0E            0F                               ASCII value");
	HFONT hFont;
	switch (iMsg)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rt);
		hEdit = CreateWindow(_T("edit"), NULL, 	
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,	
			0, 100, rt.right, rt.bottom, hwnd,
			(HMENU)IDC_EDIT, hInst, NULL
		);

		hIndex = CreateWindow(_T(""), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
			0, 0, rt.right, 25, hwnd,
			(HMENU)IDC_EDIT, hInst, NULL
		);
		break;

	case WM_SIZE:
		GetClientRect(hwnd, &rt);
		MoveWindow(hEdit, 0, 25, rt.right, rt.bottom, TRUE);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd,&lp);

		if (flag)
		{
			SetTextColor(hdc, RGB(0, 0, 255));
			if (offset_flag == FALSE)
				TextOut(hdc, 0,0, index,_tcslen(index));
			else {
				index = _T("Ofxset   Dec            00            01            02            03            04            05            06            07            08            09            0A            0B           0C            0D            0E            0F                               ASCII value");
				TextOut(hdc, 0, 0, index, _tcslen(index));
			
			}
		}
		EndPaint(hwnd, &lp);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 1:
			if (offset_flag == FALSE)
				offset_flag = TRUE;
			else
				offset_flag = FALSE;

			if (flag == TRUE)
			{
				FileRead(hEdit, filepath);
				InvalidateRgn(hwnd, NULL, TRUE);
			}
			break;
		case ID_FILEOPEN:
			memset(&OFN, 0, sizeof(OPENFILENAME));	
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hwnd;
			OFN.lpstrFilter =_T("Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0");
			OFN.lpstrFile = filepath;
			OFN.nMaxFile = 1000;
			OFN.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
			if (GetOpenFileName(&OFN) != 0) {
			}
			pStr = filepath;
			_tcscpy_s(folder, pStr);
			pStr = pStr + _tcslen(pStr) + 1;
			while (*pStr)
			{
				_tcscpy_s(filename[count++], pStr);
				pStr = pStr + _tcslen(pStr) + 1;
			}
			OFN.lpstrInitialDir = _T(".");	

			if (filepath != 0)
			{
				flag = TRUE;
				InvalidateRgn(hwnd, NULL, TRUE);
				FileRead(hEdit, filepath);
				button = CreateWindow(TEXT("button"), TEXT("OffSet"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					0, 0, 50, 25, hwnd, (HMENU)1, hInst, NULL);
			}
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam)
{
	int i, SelectWnd = 0;
	HDC hdc;
	RECT rt;
	PAINTSTRUCT ps;

	for (i = 1; i <= WndCount; i++)
	{
		if (hwnd == hwndChild[i]) {
			SelectWnd = i;
			break;
		}
	}
	switch (iMsg)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rt);
		DrawText(hdc, Winbuffer[SelectWnd], (int)_tcslen(Winbuffer[SelectWnd]),
			&rt, DT_TOP | DT_LEFT);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		return 0;
	}
	return DefMDIChildProc(hwnd, iMsg, wParam, lParam);
}