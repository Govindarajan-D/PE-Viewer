#include <Windows.h>
#include "resource.h"

#define PE_HEADER_ADDRESS 0x0000003C

LPCSTR g_className = "Main Window";
OPENFILENAME exe_file;

VOID readExecutable()
{
	DWORD checkBuffer, PE_HeaderBegin;
	HANDLE hExeFile;

	hExeFile = CreateFile(exe_file.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	SetFilePointer(hExeFile, PE_HEADER_ADDRESS, NULL, FILE_BEGIN);
	ReadFile(hExeFile, &PE_HeaderBegin,4,&checkBuffer, NULL);
}

BOOL checkExecutable()
{
	HANDLE hExeFile;
	CHAR buffer[3];
	DWORD checkBuffer;

	hExeFile = CreateFile(exe_file.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(ReadFile(hExeFile, buffer, 2, &checkBuffer, NULL))
	{
		buffer[2]='\0';
		if(strcmp(buffer, "MZ"))
		{	
			MessageBox(NULL, "Not a valid Executable file", "Error-MZ-Check", MB_ICONERROR);
			return false;
		}
		else
		{
			CloseHandle(hExeFile);
			return true;
		}
	
	}
	else
	{	
		MessageBox(NULL, "Failed to Read File", "Error-MZ-Check", MB_ICONERROR);
		return false;
	}
}

BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case ID_OK1:
					EndDialog(hWnd, ID_OK1);
					break;
			}
		}
		break;

	case WM_INITDIALOG:
		return true;
	default:
		return false;
	}
	return true;
}


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	char fileName[MAX_PATH] = "";

	ZeroMemory(&exe_file, sizeof(exe_file));
	exe_file.lpstrFile = fileName;
	exe_file.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	exe_file.hwndOwner = hWnd;
	exe_file.nMaxFile = MAX_PATH;
	exe_file.lStructSize = sizeof(OPENFILENAME);
	exe_file.lpstrFilter = "Executable files(*.exe)\0*.exe\0 All Files(*.*)\0 *.*\0";
	exe_file.lpstrDefExt = "exe";

	switch(Msg)
	{
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
	case WM_LBUTTONDOWN:
		MessageBox(hWnd, "Clicked", "Notification", MB_OK); //Can specify 0 for MB_OK
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_OPEN:
			if(!GetOpenFileName(&exe_file))
					MessageBox(NULL,"Load a executable file to view its structure", "Error-Open", MB_ICONINFORMATION);
			else
			{
				if(!checkExecutable())
					PostMessage(hWnd, WM_COMMAND, 1007,0);
				readExecutable();
			}
			break;
		case ID_EXIT:
			PostQuitMessage(0);
			break;
		case ID_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDDLG_ABOUT), hWnd, AboutDlgProc);
			break;
		}		
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
		
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR CmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG Msg;
	WNDCLASSEX wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = MainWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH) GetStockObject(COLOR_GRAYTEXT);
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wndClass.lpszClassName = g_className;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	if(!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL,"Failed to Register Window", "Error-Window", MB_ICONERROR);
	}

	//hWnd = CreateWindow(g_className, TEXT("Main Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	  hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, g_className, "PE Viewer", WS_OVERLAPPEDWINDOW, 40, 40, 400, 300, NULL, NULL, hInstance, NULL);

	if(hWnd == NULL)
	{
		MessageBox(NULL, "Failed to Create Window","Error-Window", MB_OK);
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while(GetMessage(&Msg, NULL, 0,0)> 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

