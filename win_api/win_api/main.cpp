#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <atlstr.h>
#include "resource.h"
HINSTANCE hInst;

using namespace std;


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam);

CString ConvertToHex(HWND hwnd, unsigned char* data, int size)
{
	CString returnvalue;
	CString str;
	for (int x = 0;x <size; x++)
	{
		if (((x % 16) == 0) && (x != 0))
		{
			returnvalue += _T("                 ");
			returnvalue += str;
			str = "";
			returnvalue += _T("\r\n");
		}
		CString temporary;
		CString temporary2;
		int value = (int)(data[x]);
		char c = (char)(data[x]);
		if(((x+1)%16) ==0)
			temporary.Format(L"%02X", value);
		else
			temporary.Format(L"%02X ", value);
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
		MessageBox(hwnd, filename, _T("파일열기오류"), MB_OK);
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

void FileSave(HWND hwnd, LPCTSTR filename)
{
	HANDLE hFile;
	LPTSTR buffer;
	DWORD size;
#ifdef _UNICODE
	WORD uni = 0xFEFF;
	DWORD nSize;
#endif

	hFile = CreateFile(filename, GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
#ifdef _UNICODE
	WriteFile(hFile, &uni, 2, &nSize, NULL);
#endif
	size = GetWindowTextLength(hwnd);
	buffer = new TCHAR[size + 1];
	memset(buffer, 0, (size + 1) * sizeof(TCHAR));
	size = GetWindowText(hwnd, (LPTSTR)buffer, size + 1);
	buffer[size] = NULL;

	WriteFile(hFile, buffer, size * sizeof(TCHAR), (LPDWORD)&size, NULL);

	CloseHandle(hFile);
	delete[] buffer;

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
		_T("2014270228_노현석"),
		WS_OVERLAPPEDWINDOW,
		200,
		200,
		1000,
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


#define IDC_EDIT 100 
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	RECT rt;
	OPENFILENAME OFN;
	TCHAR str[100], lpstrFile[100] = _T("");

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
		break;

	case WM_SIZE:
		GetClientRect(hwnd, &rt);
		MoveWindow(hEdit, 0, 0, rt.right, rt.bottom, TRUE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILEOPEN:
			memset(&OFN, 0, sizeof(OPENFILENAME));	// 초기화
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hwnd;
			OFN.lpstrFilter =_T("Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0");
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = _T(".");	// 초기 디렉토리
			if (GetOpenFileName(&OFN) != 0) {
			}

			FileRead(hEdit, lpstrFile);
			break;

		case ID_FILESAVE:
			FileSave(hEdit,lpstrFile);
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
