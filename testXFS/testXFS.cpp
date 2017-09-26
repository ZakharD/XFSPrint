// testXFS.cpp : print on receipt printer 

#include "stdafx.h"
#include "resource.h"
#include <xfsapi.h>
#include <xfsptr.h>

#define MAX_LOADSTRING 100
#define IDC_BUTTON1 5001
#define IDC_BUTTON2 5002

HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			

//*****************************************************/
HWND hButton1;
HWND hButton2;
const int null = 0;
char* logicalName = "ReceiptPrinter1" ;
HSERVICE h_Service; 
//*****************************************************/

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				WFSInitProc(HWND, HSERVICE&);
void				WFSStopProc(HSERVICE& hSrv);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTXFS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TESTXFS);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TESTXFS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_TESTXFS;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; 

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      50, 50, 250, 200, NULL, NULL, hInstance, NULL);
   hButton1 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Start"),
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 10, 10, 100, 100,
            hWnd, (HMENU)IDC_BUTTON1, hInst, 0);
   hButton2 = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Stop"),
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 120, 10, 100, 100,
            hWnd, (HMENU)IDC_BUTTON2, hInst, 0);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				case IDC_BUTTON1:
					WFSInitProc(hWnd, h_Service);
					break;
				case IDC_BUTTON2:
					WFSStopProc(h_Service);
					break;
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
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
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

void WFSInitProc(HWND hWnd, HSERVICE& hSrv)
{
	char* szTextOut = "WFSStartUp() performed";

	WFSVERSION wfsv;
	const DWORD RECOGNISED_VERSIONS=0X0000CCCC; //used for WFSStartUp and WFSOpen
	int const traceLevel = NULL;
	const DWORD waitTime = 60000;
	WFSVERSION lpSrvcVersion;
	LPWFSRESULT lpResult = NULL;

	HRESULT res = WFSStartUp(RECOGNISED_VERSIONS, &wfsv);
	if(res==WFS_SUCCESS)
	{
		//MessageBox(hWnd, "WFSStartUp() performed", "Operation Success", MB_ICONINFORMATION);
		res = WFSOpen ( logicalName, WFS_DEFAULT_HAPP, 	"ConsileWFS", traceLevel, waitTime, RECOGNISED_VERSIONS, &lpSrvcVersion, NULL, &hSrv);
		if(res == WFS_SUCCESS)
		{
		//	MessageBox(hWnd, "WFSOpen() performed", "Operation Success", MB_ICONINFORMATION);
			res = WFSRegister(hSrv, SERVICE_EVENTS | USER_EVENTS | EXECUTE_EVENTS, hWnd);
			if(res == WFS_SUCCESS)
			{
				//MessageBox(hWnd, "WFSRegister() performed", "Operation Success", MB_ICONINFORMATION);
				res = WFSLock(hSrv, 20000, &lpResult);
				if( res == WFS_SUCCESS)
				{
					//MessageBox(hWnd, "WFSLock() performed", "Operation Success", MB_ICONINFORMATION);
					res = WFSFreeResult(lpResult);
					if (res == WFS_SUCCESS)
					{
						//MessageBox(hWnd, "WFSFreeResult() performed", "Operation Success", MB_ICONINFORMATION);
						unsigned char data[7] = "\x0ELOCK\x0D";
						WFSPTRRAWDATA prnCmd;
						WFSPTRRAWDATA* pprnCmd = & prnCmd;

						prnCmd.wInputData = WFS_PTR_NOINPUTDATA;
						prnCmd.ulSize     = (ULONG)7;
						prnCmd.lpbData    = data;
						lpResult = NULL;
						WFSExecute(hSrv, WFS_CMD_PTR_RAW_DATA, pprnCmd, 10000, &lpResult);
						//WFSGetInfo(hSrv, WFS_PTR_PAPERFULL, NULL, 10000, &lpResult);
					}
				}
				else
				{ 	
					MessageBox(hWnd, TEXT("Error WFSLock()"), TEXT("Operation Error"), MB_ICONINFORMATION);
				}
			}
		}
		else
		{
			WFSCleanUp();
		}
		WFSClose(hSrv);
	}
	else{
		exit(1);
	}
}

void WFSStopProc(HSERVICE& hSrv)
{
	WFSUnlock(hSrv);
	WFSDeregister(hSrv, null, null);
	WFSClose(hSrv);
	WFSCleanUp();
}
