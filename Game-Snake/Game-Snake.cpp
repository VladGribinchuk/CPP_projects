//
// Game: SNAKE
// Designed by Gribinchuk Vlad, 2015
// powered uses Win API library
//

#include "stdafx.h"
#include "Game-Snake.h"

////
////
//// Global Variables:
	HINSTANCE hInst;		

//
//// App main window components:
	HWND MainWnd;
	HWND MainStatusBar;
	HWND MainBeginButton;
	HWND MainStartButton;
	HWND MainEditBox;
	HWND MainListUsers;
	HWND MainLink;
	RECT LoadingFile;
	HWND WorkWnd;
	HWND WorkStatusBar;
	HWND WorkStartButton;
	HWND WorkStopButton;
	HWND ToolButton;
	HWND SkinButton;
	HWND TableButton;
////
	HMODULE hDLLIcon;
	HICON hIcon;
	HMODULE hDLLFunc;

//
//// User components:
	std::list<_User> USER;
	_User curUSER;
	std::list<_Level> LEVEL;
	_Level curLEVEL;
	std::list<_Table> TABLE;
	DWORD curLEVELINT;
////

//
//// Interface components:
	Snake SNAKE;
	HBRUSH hBrushSnake;
	COLORREF cSkinSnake;
	COLORREF cFon;
	RECT *RGTL;
	RECT SnakeRect;
	RECT closeRect;
	RECT levelRect;
	RECT timeRect;
	RECT  pointRect;
	RECT  pointRectDown;
	RECT  pointPlusSpeed;
	RECT  pointPlusSize;
	RECT  pointPlusTime;
	INT VECTORHORIZ, VECTORVERT;
	static BOOL run;
	BOOL refresh = 1;
	
	HBRUSH hBrushFrame, hBrushBk, hBrushClose;
	HPEN hGlobalPen;
	HFONT hFont2, hFontBtn2, hFontReserve2;
	RECT WorkClientRect;
	RECT witoutSnakeRect;
	HDC memBit;
	HBITMAP hBitmap;
	Gdiplus::Image *image;
	Gdiplus::Graphics *graphics;
////

//
//// Global setting:
	BOOL isLevelChoosen;
	static BOOL isAddons1;
	static BOOL isAddons2;
	static BOOL isFileLoaded;
	int loading = 0;

	int LastLevel;
	double LevelTime;
	double BetweenTime;
	double BetweenTimeTmp;
	double TotalPoint;
	double TotalPointCopy;
	static BOOL PlusPoint;
	static BOOL isCrashSnake;
////

//
//// Windows classes 
TCHAR GlobalStr[200];
TCHAR szMainTitle[MAX_LOADSTRING] = L"UserClient v.1.1";
TCHAR szWorkTitle[MAX_LOADSTRING] = L"Snake";
TCHAR szMainWindowClass[MAX_LOADSTRING] = L"MainWindow";
TCHAR szWorkWindowClass[MAX_LOADSTRING] = L"WorkWindow";
////

//
//// Declare main functions
ATOM				RegisterMainClass(HINSTANCE hInstance);
ATOM				RegisterWorkClass(HINSTANCE hInstance);
BOOL				InitMainWindow(HINSTANCE, int);
BOOL				InitWorkWindow(HINSTANCE, int);
LRESULT CALLBACK	MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WorkWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Dialog1(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Dialog2(HWND, UINT, WPARAM, LPARAM);
VOID	CALLBACK	TimerProc(HWND, UINT, UINT_PTR, DWORD);
////

//
//// Declare users functions
	void StartGame();
	void LoadLevel();
	void DrawLevel(HWND,HDC &hdc, RECT);
	void MyRedrawWindow(HWND,HDC &hdc);
	void SendWorkStatus();
	void AddScores();
	BOOL LoadAllFiles();
	void SaveCurUser();
	void OpenExtraFile();
	unsigned WINAPI ThreadSnake(PVOID pvParam);
	typedef DWORD(WINAPI* PFN)(TCHAR*, std::list<_User>*, std::list<_Table>*);
	typedef DWORD(WINAPI* PFN2)(TCHAR*, std::list<_User>*, std::list<_Level>*, std::list<_Table>*);
	PFN pfnSaveUserFile;
	PFN2 pfnLoadUserFile;
	void CrashSnake();
	
	bool operator< (const _Table x, const _Table y){
		return x.Scores > y.Scores;
	}
////

//// MAIN FUNC, entry point
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;
	RegisterMainClass(hInstance);
	
	hInst = hInstance;
	INITCOMMONCONTROLSEX lpInitCtrls;
	lpInitCtrls.dwICC = ICC_LINK_CLASS;
	InitCommonControlsEx(&lpInitCtrls);

	if (!InitMainWindow(hInst, nCmdShow))	{
		return FALSE;
	}

	RegisterWorkClass(hInstance);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));
	
	while (GetMessage(&msg, NULL, 0, 0)){
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))	{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int) msg.wParam;
}


ATOM RegisterMainClass(HINSTANCE hInstance)
{
	//// load dll with icon for application
	hDLLIcon = LoadLibrary(_T("icondll"));
	hIcon = LoadIcon(hDLLIcon, MAKEINTRESOURCE(IDI_SNAKEICON));

	//// register class for main window
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon = LoadIcon(hDLLIcon, MAKEINTRESOURCE(IDI_SNAKEICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szMainWindowClass;
	wcex.hIconSm = LoadIcon(hDLLIcon, MAKEINTRESOURCE(IDI_SNAKEICON));
	return RegisterClassEx(&wcex);
}

BOOL InitMainWindow(HINSTANCE hInstance, int nCmdShow)
{
	//// create and show main window on center of screen
	//// main window is a UserClient window.
	//// there is a list of registred users account 
	//// and possibility of choose user account
	MainWnd = CreateWindow(szMainWindowClass, szMainTitle, 
		WS_CAPTION | WS_SYSMENU | WS_DISABLED,
		(GetSystemMetrics(SM_CXSCREEN) - MAINWINDOWWIDTH) / 2, (GetSystemMetrics(SM_CYSCREEN) - MAINWINDOWHEIGHT) / 2, MAINWINDOWWIDTH, MAINWINDOWHEIGHT, NULL, NULL, hInstance, NULL);

	if (!MainWnd){
	  MessageBox(NULL, L"Application hasnt created", L"Error", MB_ICONERROR);
      return FALSE;
	}

	ShowWindow(MainWnd, nCmdShow);
	UpdateWindow(MainWnd);

	//// load follow files
	//// user.cfg - configuration of registred users
	//// scores.bin	- file which save users results
	//// level.bin - rooms
	//// addon.dat
	//// addon2.dat - file for extra "mystery" skin window
	if (!LoadAllFiles())
		return FALSE;
	EnableWindow(MainWnd, TRUE);

	return TRUE;
}

ATOM RegisterWorkClass(HINSTANCE hInstance)
{
	WNDCLASSEX cex = { 0 };
	cex.cbSize = sizeof(WNDCLASSEX);
	cex.lpfnWndProc = WorkWndProc;
	cex.hInstance = hInst;
	cex.hCursor = LoadCursor(NULL, IDC_ARROW);
	cex.hbrBackground = (HBRUSH)(COLOR_MENUHILIGHT);
	cex.lpszClassName = szWorkWindowClass;
	return RegisterClassEx(&cex);
}

BOOL InitWorkWindow(HINSTANCE hInstance, int nCmdShow)
{
	//// work window - is a main window gameplay
	//// there is an area with current level room, 
	//// an area with setting is placed bottom
	WorkWnd = CreateWindow(szWorkWindowClass, szWorkTitle,
		WS_OVERLAPPED | WS_POPUPWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - WORKWINDOWWIDTH) / 2, (GetSystemMetrics(SM_CYSCREEN) - WORKWINDOWHEIGHT) / 2, WORKWINDOWWIDTH, WORKWINDOWHEIGHT, NULL, NULL, hInstance, NULL);
	if (!WorkWnd){
		MessageBox(NULL, L"Application hasnt created", L"Error", MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(WorkWnd, SW_SHOWDEFAULT);
	MyRedrawWindow(WorkWnd, memBit);
	InvalidateRect(WorkWnd, &WorkClientRect, 1);
	UpdateWindow(WorkWnd);

	return TRUE;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static HFONT hFont;
	static HFONT hFontReserve;
	static HFONT hFontBtn;
	static HBRUSH hBrush;
	RECT clientRect;
	RECT listRect;
	TCHAR newUser[50];
	TCHAR loadStr[200];
	static BOOL UniqueName;
	static BOOL isNameWriten;
	std::list<_User>::iterator it;
	_User tmpUser;
	int i;

	HICON hIcon;

	switch (message)
	{
	case WM_CREATE:
		
		GetClientRect(hWnd, &clientRect);

		MainListUsers = CreateWindow(WC_LISTBOX, NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_AUTOHSCROLL,
			(clientRect.right - 250) / 2, 20, 250, 200, hWnd, (HMENU)ID_CLK_LISTBOX, hInst, NULL);
		SetRect(&listRect, (clientRect.right - 250) / 2, 20, 250, 200);
		
		MainBeginButton = CreateWindow(WC_BUTTON, _T("Create"),
			WS_CHILD | WS_VISIBLE,
			(clientRect.right - 250) / 2, 260, 250, 25, hWnd, (HMENU)ID_BEGIN_BUTTON, hInst, NULL);
		
		MainEditBox = CreateWindow(WC_EDIT, _T(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			(clientRect.right - 250) / 2, 230, 250, 25, hWnd, NULL, hInst, NULL);

		MainStartButton = CreateWindow(WC_BUTTON, _T("Start"),
			WS_CHILD | WS_VISIBLE | WS_DISABLED,
			(clientRect.right - 250) / 2, 450, 250, 50, hWnd, (HMENU)ID_START_BUTTON, hInst, NULL);

		MainLink = CreateWindow(WC_LINK, L"<A HREF=\"http://vk.com/vlad_gribinchook\">VG</A> | <A ID=\"idInfo\">About</A>",
			WS_CHILD | WS_VISIBLE ,
			(clientRect.right - 250) / 2, 530, 250, 20, hWnd, NULL, hInst, NULL);;
		
		SetRect(&LoadingFile, (clientRect.right - 250) / 2, 290, (clientRect.right - 250) / 2 + 250, 290 + 20);

		hFontBtn = CreateFont(14, 0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Segoe UI");
		hFont = CreateFont(18, 0, 0, 0, 500, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Segoe UI");
		hFontReserve = CreateFont(18, 0, 0, 0, 500, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Arial");

		break;
	case WM_ENABLE:
		if (!isNameWriten)
		{
			it = USER.begin();
			while (it != USER.end())
			{
				ListBox_AddString(MainListUsers, (*it).UserName);
				it++;
			}
		}
		isNameWriten = TRUE;
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case ID_CLK_LISTBOX:

			if (ListBox_GetCount(MainListUsers) < 1){
				ListBox_SetCurSel(MainListUsers, 0);
			}
			EnableWindow(MainStartButton, TRUE);		
		break;

		case ID_BEGIN_BUTTON:
			if (Edit_GetText(MainEditBox, newUser, sizeof(newUser)))
			{
				it = USER.begin();
				while (it != USER.end())
				{
					if (!_tcscmp((*it).UserName, newUser))
					{
						UniqueName = TRUE;
						break;
					} else
						UniqueName = FALSE;
					it++;
				}
				
				if (!UniqueName)
				{
					ListBox_AddString(MainListUsers, newUser);

					tmpUser.ProgressLevel = 1;
					tmpUser.isAllLevelOpen = 0;
					tmpUser.isExtraSkin = 0;
					tmpUser.SKIN = 1;
					tmpUser.SNAKESPEED = 200;
					_tcscpy_s(tmpUser.UserName, newUser);
					for (i = 0; i < LEVELNUM;i++)
						tmpUser.records[i] = 0;
					USER.push_back(tmpUser);

					curUSER.ProgressLevel = tmpUser.ProgressLevel;
					curUSER.isAllLevelOpen = tmpUser.isAllLevelOpen;
					curUSER.isExtraSkin = tmpUser.isExtraSkin;
					curUSER.SKIN = tmpUser.SKIN;
					curUSER.SNAKESPEED = tmpUser.SNAKESPEED;
					for (i = 0; i < LEVELNUM; i++)
						curUSER.records[i] = tmpUser.records[i];
					_tcscpy_s(curUSER.UserName, tmpUser.UserName);

					StartGame();
				} else
					MessageBox(hWnd, L"Enter unique name!", L"Find equal user name", MB_OK);
				Edit_SetText(MainEditBox, L"\0");

			}
			break;
		
		case ID_START_BUTTON:
			if (ListBox_GetCurSel(MainListUsers) == -1)
				MessageBox(NULL, L"Please, select exist user account or create new account", L"Select account", MB_OK);
			else {

				ListBox_GetText(MainListUsers, ListBox_GetCurSel(MainListUsers), tmpUser.UserName);

				it = USER.begin();
				while ((it != USER.end()) && (_tcscmp((*it).UserName, tmpUser.UserName))) { it++; }
				
				curUSER.ProgressLevel = (*it).ProgressLevel;
				curUSER.isAllLevelOpen = (*it).isAllLevelOpen;
				curUSER.isExtraSkin = (*it).isExtraSkin;
				curUSER.SKIN = (*it).SKIN;
				curUSER.SNAKESPEED = (*it).SNAKESPEED;
				for (i = 0; i < LEVELNUM; i++)
					curUSER.records[i] = (*it).records[i];
				_tcscpy_s(curUSER.UserName, (*it).UserName);
				StartGame();
			}
			break;
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	
		
	case WM_CTLCOLORLISTBOX:
		SetTextColor((HDC)wParam, RGB(0,102,204));
		SetBkMode((HDC)wParam, TRANSPARENT);
		if (hBrush)
			DeleteObject(hBrush);
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		return (LRESULT)hBrush;

	case WM_CTLCOLORBTN:
		SetTextColor((HDC)wParam, RGB(0, 0, 0));
		SetBkMode((HDC)wParam, TRANSPARENT);
		break;
	case WM_CTLCOLOREDIT:
		SetTextColor((HDC)wParam, RGB(0, 102, 204));
		SetBkMode((HDC)wParam, TRANSPARENT);
		break;
	case WM_CTLCOLORSTATIC:
		SetTextColor((HDC)wParam, RGB(120, 120, 120));
		SetBkMode((HDC)wParam, TRANSPARENT);
		if (hBrush)
			DeleteObject(hBrush);
		hBrush = CreateSolidBrush(RGB(248, 248, 248));
		return (LRESULT)hBrush;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &clientRect);
	
		if (hFont == NULL)
			if (hFontReserve) 
				SelectObject(hdc, hFontReserve); 
			else
				SelectObject(hdc, GetStockFont(SYSTEM_FONT));
		else
			SelectObject(hdc, hFont);

		SendMessage(MainListUsers, WM_SETFONT, (WPARAM)hFont, 1);
		SendMessage(MainBeginButton, WM_SETFONT, (WPARAM)hFontBtn, 1);
		SendMessage(MainStartButton, WM_SETFONT, (WPARAM)hFontBtn, 1);
		SendMessage(MainEditBox, WM_SETFONT, (WPARAM)hFont, 1);
		SendMessage(MainLink, WM_SETFONT, (WPARAM)hFont, 1);
		
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(120, 120, 120));
		if (hBrush) DeleteObject((HBRUSH)hBrush);
		hBrush = CreateSolidBrush(RGB(248, 248, 248));
		SelectObject(hdc, hBrush);
		FillRect(hdc, &clientRect, hBrush);

		DrawText(hdc, L"Select USER from user's account list:\0", 40, &clientRect, DT_CENTER); 
		TextOut(hdc, (clientRect.right - 250) / 2, 210, _T("Sign in or create new account.\0"), 30);
		SelectObject(hdc, hFontBtn);
		TextOut(hdc, (clientRect.right - 250) / 2, 500, _T("The \'Snake\' app, current version - 1.1 \0"), 40);
		TextOut(hdc, (clientRect.right - 250) / 2, 515, _T("All rights reserved © 2015, Vlad Gribichuk.\0"), 44); 
		
		if (loading == 1){
			_stprintf_s(loadStr, L"Loading...%s\0", L"filedll.dll");
			DrawText(hdc, loadStr, 21, &LoadingFile, DT_LEFT);
		}
		if (loading == 2){
			_stprintf_s(loadStr, L"Loading...%s\0", L"user.cfg");
			DrawText(hdc, loadStr, 18, &LoadingFile, DT_LEFT);
		}
		if (loading == 3){
			_stprintf_s(loadStr, L"Loading...%s\0", L"level.bin");
			DrawText(hdc, loadStr, 19, &LoadingFile, DT_LEFT);
		}
		if ((loading == 4) && isAddons1){
			_stprintf_s(loadStr, L"Loading...%s\0", L"addon.key");
			DrawText(hdc, loadStr, 19, &LoadingFile, DT_LEFT);
		}
		if ((loading == 5) && isAddons2){
			_stprintf_s(loadStr, L"Loading...%s\0", L"addon2.key");
			DrawText(hdc, loadStr, 20, &LoadingFile, DT_LEFT);
		}
		if (loading == 6){
			_stprintf_s(loadStr, L"Loading...%s\0", L"scores.key");
			DrawText(hdc, loadStr, 20, &LoadingFile, DT_LEFT);
		}
		if (loading == 7){
			_stprintf_s(loadStr, L"All files load succesfull!");
			DrawText(hdc, loadStr, 26, &LoadingFile, DT_LEFT);
		}
		
		EndPaint(hWnd, &ps);
		break;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code){
		case NM_CLICK:          // Fall through to the next case.
		case NM_RETURN:
			PNMLINK pNMLink = (PNMLINK)lParam;
			LITEM   item = pNMLink->item;

			if ((((LPNMHDR)lParam)->hwndFrom == MainLink) && (item.iLink == 0))
				ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
			else if (wcscmp(item.szID, L"idInfo") == 0)
				MessageBox(NULL, L"Author - Vlad Gribinchuk\nVersion - 1.1", L"About", MB_OK);
			break;
		}
		break;

	case WM_DESTROY:
		SaveCurUser();

		if (!USER.empty())
			USER.clear();
		if (!LEVEL.empty())
			LEVEL.clear();
		if (!TABLE.empty())
			TABLE.clear();

		if (hFont) 
			DeleteObject((HFONT)hFont);
		if (hFontBtn)
			DeleteObject((HFONT)hFontBtn);
		if (hFontReserve)
			DeleteObject((HFONT)hFontReserve);
		if (hBrush)
			DeleteObject((HBRUSH)hBrush);

		FreeLibrary(hDLLIcon);
		FreeLibrary(hDLLFunc);

		GlobalFree((HGLOBAL)RGTL);

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WorkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	RECT cl2;
	int i, s;
	static BOOL close,client;
	static POINT coord;
	static POINT mousexy;
	static int relx, rely;
	static int sx, sy;
	TCHAR status[150];
	static CHOOSECOLOR ccs;
	static COLORREF acrCustClr[16];
	static HBRUSH hBrush;

	switch (message)
	{
	case WM_CREATE:
		srand(time(NULL));

		RGTL = (RECT*)GlobalAlloc(GPTR, MAXNODESNAKE * sizeof(RECT));

		SetRect(&SnakeRect, BEGINMAPX, BEGINMAPY, MAPX + BEGINMAPX, MAPY + BEGINMAPY);
		SetRect(&closeRect, BEGINMAPX + MAPX - 31, 0, BEGINMAPX + MAPX + BEGINMAPX - 5, 2 + 16);
		SetRect(&levelRect, BEGINMAPX, BEGINMAPY + MAPY + 5, BEGINMAPX + MAPX / 4, BEGINMAPY + MAPY + 5 + 20);
		SetRect(&timeRect, BEGINMAPX + MAPX / 4, BEGINMAPY + MAPY + 5, MAPX/2 + BEGINMAPX, BEGINMAPY + MAPY + 5 + 20);
		SetRect(&pointRect, MAPX / 2 + BEGINMAPX, BEGINMAPY + MAPY + 5, MAPX + BEGINMAPX, BEGINMAPY + MAPY + 5 + 20);
		SetRect(&pointRectDown, MAPX / 2 + BEGINMAPX, BEGINMAPY + MAPY + 5 + 20, MAPX + BEGINMAPX, BEGINMAPY + MAPY + 5 + 40);
		SetRect(&pointPlusSpeed, MAPX / 2 + BEGINMAPX + 50, BEGINMAPY + MAPY + 45, MAPX + BEGINMAPX, BEGINMAPY + MAPY + 45 + 15);
		SetRect(&pointPlusSize, MAPX / 2 + BEGINMAPX + 60, BEGINMAPY + MAPY + 60, MAPX + BEGINMAPX, BEGINMAPY + MAPY + 60 + 15);
		SetRect(&pointPlusTime, MAPX / 2 + BEGINMAPX + 70, BEGINMAPY + MAPY + 75, MAPX + BEGINMAPX, BEGINMAPY + MAPY + 75 + 15);
		WorkStatusBar = CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD |
			WS_VISIBLE, 0, 0, 20, 20, hWnd, NULL, hInst, NULL);

		WorkStartButton = CreateWindow(WC_BUTTON, _T("R U N"),
			WS_CHILD | WS_VISIBLE,
			BEGINMAPX, MAPY + BEGINMAPY + 30, MAPX / 4 -10 , 30, hWnd, (HMENU)ID_WORK_START_BUTTON, hInst, NULL);

		WorkStopButton = CreateWindow(WC_BUTTON, _T("S T O P"),
			WS_CHILD | WS_VISIBLE | WS_DISABLED,
			BEGINMAPX + MAPX / 4 + 10, MAPY + BEGINMAPY + 30, MAPX / 4 - 10, 30, hWnd, (HMENU)ID_WORK_STOP_BUTTON, hInst, NULL);
		
		ToolButton = CreateWindow(WC_BUTTON, _T("open settings"),
			WS_CHILD | WS_VISIBLE,
			BEGINMAPX, MAPY + BEGINMAPY + 100, MAPX / 4 - 10, 20, hWnd, (HMENU)ID_SETTING_OPEN, hInst, NULL);
		
		SkinButton = CreateWindow(WC_BUTTON, _T("skin snake"),
			WS_CHILD | WS_VISIBLE,
			BEGINMAPX, MAPY + BEGINMAPY + 100 + 25, MAPX / 4 - 10, 20, hWnd, (HMENU)ID_SKIN_SNAKE, hInst, NULL);

		TableButton = CreateWindow(WC_BUTTON, _T("scores table"),
			WS_CHILD | WS_VISIBLE,
			BEGINMAPX + MAPX / 4 + 10, MAPY + BEGINMAPY + 100, MAPX / 4 - 10, 45, hWnd, (HMENU)ID_SCORES_TABLE, hInst, NULL);

		hFontBtn2 = CreateFont(14, 0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Segoe UI");
		hFont2 = CreateFont(18, 0, 0, 0, 500, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Segoe UI");
		hFontReserve2 = CreateFont(18, 0, 0, 0, 500, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Arial");
		
		cFon = RGB(rand() % 256, rand() % 256, rand() % 256);
		hBrushClose = CreateSolidBrush(RGB(240, 100, 100));

		ccs.lStructSize = sizeof(CHOOSECOLOR);
		ccs.hwndOwner = hWnd;
		ccs.rgbResult = RGB(255, 255, 255);
		ccs.Flags = CC_RGBINIT | CC_FULLOPEN;
		ccs.lpCustColors = (LPDWORD)acrCustClr;

		
		hdc = GetDC(hWnd);
		memBit = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		SelectObject(memBit, hBitmap);
		PatBlt(memBit, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), WHITENESS);
		ReleaseDC(hWnd, hdc);

		break;

	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		relx = GET_X_LPARAM(lParam);
		rely = GET_Y_LPARAM(lParam);
		break;
	case WM_MOUSEMOVE:
		GetCursorPos(&coord);
		ScreenToClient(hWnd, &coord);
		cl2 = closeRect;
		cl2.top = 2;
		if (PtInRect(&cl2, coord))
		{
			refresh = FALSE;
			if (client)
			{
				MyRedrawWindow(hWnd, memBit);
				InvalidateRect(hWnd, &closeRect, 0);
				UpdateWindow(hWnd);
			}
			client = FALSE;
			close = TRUE;
		}		
		else {
			refresh = TRUE;
			if (close)
			{
				MyRedrawWindow(hWnd, memBit);
				InvalidateRect(hWnd, &closeRect, 0);
				UpdateWindow(hWnd);
			}
			client = TRUE;
			close = FALSE;
		}

		if (wParam == MK_LBUTTON)
		{	
			GetCursorPos(&coord);
			MoveWindow(hWnd, coord.x - relx, coord.y - rely, WORKWINDOWWIDTH, WORKWINDOWHEIGHT, TRUE);
			UpdateWindow(hWnd);
			if (curUSER.SKIN == 3)	{
				cFon = RGB(rand() % 256, rand() % 256, rand() % 256);
				MyRedrawWindow(hWnd, memBit);
				InvalidateRect(hWnd, NULL, TRUE);}
		}
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		GetCursorPos(&coord);
		ScreenToClient(hWnd, &coord);
		if (PtInRect(&closeRect, coord))
			SendMessage(hWnd, WM_CLOSE, wParam, lParam);
		break;
	

	case WM_CTLCOLORBTN:
		SetTextColor((HDC)wParam, RGB(0, 0, 0));
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)hBrushBk;
		break;

	case WM_CTLCOLORSTATIC:
		if (refresh)
			SetTextColor((HDC)wParam, RGB(200, 0, 0));
		else
			SetTextColor((HDC)wParam, RGB(0, 0, 0));
		SetBkMode((HDC)wParam, TRANSPARENT);
		if (hBrushBk)
			DeleteObject(hBrushBk);
		hBrushBk = CreateSolidBrush(RGB(248, 248, 248));
		return (LRESULT)hBrushBk;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, 800, 600, memBit, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{

		case ID_WORK_START_BUTTON:
			run = TRUE;
			SendWorkStatus();
			EnableWindow(WorkStopButton, TRUE);
			EnableWindow(WorkStartButton, FALSE);
			SetTimer(hWnd,10, curUSER.SNAKESPEED, TimerProc);
			SetTimer(hWnd, 22, 100, NULL);
			SetTimer(hWnd, 23, 100, NULL);
			SetFocus(hWnd);
			break;

		case ID_WORK_STOP_BUTTON:
			run = FALSE;
			SendWorkStatus();
			EnableWindow(WorkStartButton, TRUE);
			EnableWindow(WorkStopButton, FALSE);
			KillTimer(hWnd, 10);
			KillTimer(hWnd, 22);
			KillTimer(hWnd, 23);
			SetFocus(hWnd);
			break;

		case ID_SETTING_OPEN:
			run = FALSE;
			SendMessage(hWnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dialog1);
			break;

		case ID_SKIN_SNAKE:
			run = FALSE;
			SendMessage(hWnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0);
			if (ChooseColor(&ccs))
				cSkinSnake = ccs.rgbResult;
			MyRedrawWindow(hWnd, memBit);
			InvalidateRect(hWnd, &timeRect, 1);
			UpdateWindow(hWnd);
			break;

		case ID_SCORES_TABLE:
			run = FALSE;
			TABLE.sort();
			SendMessage(hWnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, Dialog2);

			break;
		default:
			break;
		}
	break;

	case WM_TIMER:
		if (wParam == 22){
			LevelTime++;
			MyRedrawWindow(hWnd, memBit);
			InvalidateRect(hWnd, &timeRect, 0);
			UpdateWindow(hWnd);
		}
		if (wParam == 23){
			BetweenTime++;
		}
		break;
	case WM_KEYDOWN:

		if (run)
		{
		switch (wParam) {
			case VK_LEFT:
				VECTORHORIZ = -1;
				VECTORVERT = 0;
				break;
			case VK_RIGHT:
				VECTORHORIZ = 1;
				VECTORVERT = 0;
				break;
			case VK_UP:
				VECTORHORIZ = 0;
				VECTORVERT = -1;
				break;
			case VK_DOWN:
				VECTORHORIZ = 0;
				VECTORVERT = 1;
				break;
			}	
		}

		if (wParam == VK_SPACE){
			if (run)
				SendMessage(hWnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0); 
			else
				SendMessage(hWnd, WM_COMMAND, ID_WORK_START_BUTTON, 0);
		}
		

		break;
	case WM_CLOSE:
		SendMessage(hWnd, WM_DESTROY, wParam, lParam);
		DestroyWindow(hWnd);

		SaveCurUser();
		if (!USER.empty())
			USER.clear();
		pfnLoadUserFile(L"user.cfg", &USER, &LEVEL, &TABLE);

		ShowWindow(MainWnd, SW_SHOWDEFAULT);
		break;
	case WM_DESTROY:
		if (hFont2)
			DeleteObject((HFONT)hFont2);
		if (hFontBtn2)
			DeleteObject((HFONT)hFontBtn2);
		if (hFontReserve2)
			DeleteObject((HFONT)hFontReserve2);
		if (hBrushBk)
			DeleteObject((HBRUSH)hBrushBk);
		if (hBrushFrame)
			DeleteObject((HBRUSH)hBrushFrame);
		if (hBrushClose)
			DeleteObject((HBRUSH)hBrushClose);
		break;

	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MyRedrawWindow(HWND hWnd, HDC &hdc)
{
	GetClientRect(hWnd, &WorkClientRect);
	if (hBrushBk) DeleteObject(hBrushBk);
	if (hBrushFrame) DeleteObject(hBrushFrame);
	switch (curUSER.SKIN)
	{
	case 0:
		hBrushBk = CreateSolidBrush(RGB(255, 255, 255));
		hBrushFrame = CreateSolidBrush(RGB(200, 200, 220));
		break;
	case 1:
		hBrushBk = CreateSolidBrush(RGB(240, 240, 240));
		hBrushFrame = CreateSolidBrush(RGB(120, 120, 120));
		SetTextColor(hdc, RGB(200, 200, 200));
		break;
	case 2:
		hBrushBk = CreateSolidBrush(RGB(25, 25, 25));
		hBrushFrame = CreateSolidBrush(RGB(230, 230, 230));
		break;
	case 3:
		hBrushBk = CreateSolidBrush(cFon);
		hBrushFrame = CreateSolidBrush(RGB(0, 0, 0));
		break;
	case 4:
		hBrushBk = CreateSolidBrush(RGB(0,0,0));
		cSkinSnake = RGB(255, 255, 255);
		graphics = new Gdiplus::Graphics(memBit);
		image = new Gdiplus::Image(L"extra.png");
		graphics->DrawImage(image, 0, 0, 800, 600);
		delete graphics;
		delete image;
		break;
	}


	if (curUSER.SKIN != 4){
		SelectObject(hdc, hBrushBk);
		FillRect(hdc, &WorkClientRect, hBrushBk);
	}

	FrameRect(hdc, &SnakeRect, hBrushFrame);

	if (hFont2 == NULL)
		if (hFontReserve2)
			SelectObject(hdc, hFontReserve2);
		else
			SelectObject(hdc, GetStockFont(SYSTEM_FONT));
	else
		SelectObject(hdc, hFont2);

	SetBkMode(hdc, TRANSPARENT);
	SendMessage(WorkStartButton, WM_SETFONT, (WPARAM)hFontBtn2, 1);
	SendMessage(WorkStopButton, WM_SETFONT, (WPARAM)hFontBtn2, 1);
	SendMessage(ToolButton, WM_SETFONT, (WPARAM)hFontBtn2, 1);
	SendMessage(SkinButton, WM_SETFONT, (WPARAM)hFontBtn2, 1);
	SendMessage(TableButton, WM_SETFONT, (WPARAM)hFontBtn2, 1);

	if (!refresh) {
		SetTextColor(hdc, RGB(255, 255, 255));
		FillRect(hdc, &closeRect, hBrushClose);
	}
	else {
		if (curUSER.SKIN != 4)
			FillRect(hdc, &closeRect, hBrushBk);
		SetTextColor(hdc, RGB(200, 200, 200));
	}

	DrawText(hdc, L"close", 5, &closeRect, DT_CENTER);

	SetTextColor(hdc, RGB(200, 200, 200));
	_stprintf_s(GlobalStr, _T("LEVEL: %d | %s"), curLEVEL.LevelNum, curLEVEL.LevelName);
	DrawText(hdc, GlobalStr, _tcslen(GlobalStr), &levelRect, DT_CENTER);
	_stprintf_s(GlobalStr, _T("TIME: %.1f s"), LevelTime / 10);
	DrawText(hdc, GlobalStr, _tcslen(GlobalStr), &timeRect, DT_CENTER);
	_stprintf_s(GlobalStr, _T("Units of food was eaten: %d | %d "), SNAKE.GetCountNode() - BEGINNODESNAKE, curLEVEL.MaxStuff);
	DrawText(hdc, GlobalStr, _tcslen(GlobalStr), &pointRect, DT_CENTER);
	_stprintf_s(GlobalStr, _T("POINT: %.0f"), TotalPoint);
	DrawText(hdc, GlobalStr, _tcslen(GlobalStr), &pointRectDown, DT_CENTER);

	if (PlusPoint)
	{
		SetTextColor(hdc, RGB(255, 0, 128));
		_stprintf_s(GlobalStr, _T("+%.0f SPEED POINTS"), (double)(100 * 1000 / curUSER.SNAKESPEED), (double)(1000 * SNAKE.GetCountNode()));
		DrawText(hdc, GlobalStr, _tcslen(GlobalStr), &pointPlusSpeed, DT_LEFT);
		_stprintf_s(GlobalStr, _T("+%0.f SIZE SNAKE POINTS"), (double)(1000 * SNAKE.GetCountNode()));
		DrawText(hdc, GlobalStr, _tcslen(GlobalStr), &pointPlusSize, DT_LEFT);
		_stprintf_s(GlobalStr, _T("+%0.f TIME BONUS"), (double)(100 / BetweenTimeTmp * 100));
		DrawText(hdc, GlobalStr, _tcslen(GlobalStr), &pointPlusTime, DT_LEFT);
		SetTextColor(hdc, RGB(200, 200, 200));
	}

	_stprintf_s(GlobalStr, _T("Press SPACE to start or stop"));
	TextOut(hdc, BEGINMAPX, MAPY + BEGINMAPY + 30 + 30, GlobalStr, 28);	

	DrawLevel(hWnd, hdc, SnakeRect); 
}

INT_PTR CALLBACK Dialog1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	
	static int radio, slider, level;
	static HWND hSlider, hEdit, hList;

	TCHAR textKEY[100];
	TCHAR levelSTR[150];
	std::list<_Level>::iterator it;
	
	switch (message)
	{
	case WM_INITDIALOG:

		SetDlgItemText(hDlg, IDC_LEVEL, L"Select level:");
		SetDlgItemText(hDlg, IDC_SPEED, L"Set snake's run speed:");
		SetDlgItemText(hDlg, IDC_SKIN, L"Select skin:");
		
		hList = GetDlgItem(hDlg, IDC_LIST1);

		it = LEVEL.begin();
		for (int i = 0; i < curUSER.ProgressLevel; i++)
		{
			_stprintf_s(levelSTR, L"LEVEL %d | %s", (*it).LevelNum, (*it).LevelName);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)levelSTR);
			it++;
		}
		
		hSlider = GetDlgItem(hDlg, IDC_SLIDER1);
		SendMessage(hSlider, TBM_SETRANGEMIN, 0, 10);
		SendMessage(hSlider, TBM_SETRANGEMAX, 0, 200);
		SendMessage(hSlider, TBM_SETPOS, 1, curUSER.SNAKESPEED);
		SetDlgItemInt(hDlg, IDC_SPEEDINT, curUSER.SNAKESPEED, 0);
		if (curUSER.SNAKESPEED > 150){
			SetDlgItemText(hDlg, IDC_COMSPEED, L"You really are a beginner!");
		} else
			if (curUSER.SNAKESPEED > 120){
				SetDlgItemText(hDlg, IDC_COMSPEED, L"Slowly...");
			} else
				if (curUSER.SNAKESPEED > 100){
					SetDlgItemText(hDlg, IDC_COMSPEED, L"Still slowly...");
				}
				else
					if (curUSER.SNAKESPEED > 60){
						SetDlgItemText(hDlg, IDC_COMSPEED, L"Not bad!..but not enough...");
					}
					else
						if (curUSER.SNAKESPEED > 50){
							SetDlgItemText(hDlg, IDC_COMSPEED, L"Wow, high speed!");
						}
						else
							if (curUSER.SNAKESPEED > 30){
								SetDlgItemText(hDlg, IDC_COMSPEED, L"Very high speed!");
							}
							else
								if (curUSER.SNAKESPEED > 20){
									SetDlgItemText(hDlg, IDC_COMSPEED, L"Are you sure? It\'s extremely!");
								} else	
									SetDlgItemText(hDlg, IDC_COMSPEED, L"Snakes Guru");
		if(curUSER.isExtraSkin) 
			EnableWindow(GetDlgItem(hDlg, IDC_RADIO5),TRUE);
		else
			EnableWindow(GetDlgItem(hDlg, IDC_RADIO5), FALSE);

		radio = curUSER.SKIN;
		CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO5, IDC_RADIO1 + radio);
		break;

	case WM_HSCROLL:
		slider = LOWORD(SendMessage(hSlider, TBM_GETPOS, 0, 0));
		curUSER.SNAKESPEED = slider;
		SetDlgItemInt(hDlg, IDC_SPEEDINT, curUSER.SNAKESPEED, 0);
		if (curUSER.SNAKESPEED > 150){
			SetDlgItemText(hDlg, IDC_COMSPEED, L"You really are a beginner!");
		}
		else
			if (curUSER.SNAKESPEED > 120){
				SetDlgItemText(hDlg, IDC_COMSPEED, L"Slowly...");
			}
			else
				if (curUSER.SNAKESPEED > 100){
					SetDlgItemText(hDlg, IDC_COMSPEED, L"Still slowly...");
				}
				else
					if (curUSER.SNAKESPEED > 60){
						SetDlgItemText(hDlg, IDC_COMSPEED, L"Not bad!..but not enough...");
					}
					else
						if (curUSER.SNAKESPEED > 50){
							SetDlgItemText(hDlg, IDC_COMSPEED, L"Wow, high speed!");
						}
						else
							if (curUSER.SNAKESPEED > 30){
								SetDlgItemText(hDlg, IDC_COMSPEED, L"Very high speed!");
							}
							else
								if (curUSER.SNAKESPEED > 20){
									SetDlgItemText(hDlg, IDC_COMSPEED, L"Are you sure? It\'s extremely!");
								}
								else
									SetDlgItemText(hDlg, IDC_COMSPEED, L"Snakes Guru");
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCHECKKEY:
			hEdit = GetDlgItem(hDlg, IDC_EDIT1);
			GetDlgItemText(hDlg, IDC_EDIT1, textKEY, 256);
			if (!curUSER.isExtraSkin)
			{
				if (!_tcscmp(textKEY, _T("AAAA-BBBA")))
				{
					if (isAddons1 && isAddons2) {
						EnableWindow(GetDlgItem(hDlg, IDC_RADIO5), TRUE);
						curUSER.isExtraSkin = 1;
					}
					
				}
			}		
			break;		
		case IDC_RADIO1: radio = 0; break;
		case IDC_RADIO2: radio = 1; break;
		case IDC_RADIO3: radio = 2; break;
		case IDC_RADIO4: radio = 3; break;
		case IDC_RADIO5: radio = 4; break;
		case IDOK:
			level = SendMessage(hList, LB_GETCURSEL, 0, 0);
			if (level == -1)
				//LoadLevel()
				;
			else {
				isLevelChoosen = TRUE;
				curLEVEL.LevelNum = level + 1;
				LoadLevel();
			}
			if (radio == 4)	{
				if (isAddons1 && isAddons2)
					OpenExtraFile();
				else {
					MessageBox(NULL, L"Extra skin is unavailable", L"Files dont find", MB_OK);
					radio = 0;
				}
			}
			
			curUSER.SKIN = radio;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			SetFocus(WorkWnd);
			MyRedrawWindow(GetParent(hDlg), memBit);
			InvalidateRect(GetParent(hDlg), NULL, 1);
			UpdateWindow(GetParent(hDlg));
			if (isLevelChoosen)
				EnableWindow(WorkStartButton, TRUE);
			else
				EnableWindow(WorkStartButton, FALSE);
			break;
		default: return FALSE;
		}
		return TRUE;
	default: return FALSE;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Dialog2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	
	static HWND hList;

	TCHAR str[200];
	std::list<_Table>::iterator it;

	switch (message)
	{
	case WM_INITDIALOG:
		
		hList = GetDlgItem(hDlg, IDC_LIST1);
		it = TABLE.begin();
		while (it != TABLE.end()) {
			_stprintf_s(str, L"USER \"%s\" | LEVEL #%d | SCORES %.0f", (*it).UserName, (*it).LevelNum, (*it).Scores);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)str);
			it++;
		}

		return TRUE;
	case WM_COMMAND:
		
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
		case IDOK: 
			EndDialog(hDlg, 0);
			SetFocus(WorkWnd);
			break;
		case IDC_BUTTON1:
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)L"All data will be erased");
			TABLE.clear();
			break;
		default: return FALSE;
		}

		return TRUE;
	default: return FALSE;
	}
	return FALSE;
}

void OpenExtraFile()
{
	HANDLE hFile;
	HANDLE hFile2;
	HANDLE hNewFile;

	DWORD dwBytesRead;
	DWORD dwBytesWritten;
	BOOL bRes;
	DWORD dwPtr;

	hFile = CreateFile(_T("addon.key"),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
	hFile2 = CreateFile(_T("addon2.key"),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
	hNewFile = CreateFile(_T("extra.png"),
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_HIDDEN,
			NULL);
	
	DWORD BUFSIZES = 1097663;
	BYTE *chBuf2 = new BYTE[250000];
			if (ReadFile(hFile, chBuf2, 250000, &dwBytesRead, NULL))	{
				bRes = WriteFile(hNewFile,
					chBuf2,
					dwBytesRead,
					&dwBytesWritten,
					NULL);
			}

	
	BYTE *chBuf3 = new BYTE[BUFSIZES - 250000];

		if (ReadFile(hFile2, chBuf3, BUFSIZES - 250000, &dwBytesRead, NULL))	{
			bRes = WriteFile(hNewFile,
				chBuf3,
				dwBytesRead,
				&dwBytesWritten,
				NULL);
		}

		delete[] chBuf2;
		delete[] chBuf3;
	CloseHandle(hFile);
	CloseHandle(hFile2);
	CloseHandle(hNewFile);
}

void StartGame()
{
	ShowWindow(MainWnd, SW_HIDE);
	if (!InitWorkWindow(hInst, SW_SHOWDEFAULT)){
		DestroyWindow(MainWnd);
	}

	SetClassLong(WorkWnd, GCL_HICON, (LONG)hIcon);
	SetClassLong(WorkWnd, GCL_HICONSM, (LONG)hIcon);
	
	LastLevel = LEVEL.size();
	
	if (curUSER.isAllLevelOpen == 1)
		curLEVEL.LevelNum = 1;
	else
		curLEVEL.LevelNum = curUSER.ProgressLevel;
	
	if (curUSER.isExtraSkin)
		OpenExtraFile();
	
	isLevelChoosen = TRUE;

	LoadLevel();
}

void LoadLevel()
{
	std::list<_Level>::iterator it = LEVEL.begin();
	while ((*it).LevelNum != curLEVEL.LevelNum) { it++; }
	curLEVEL = *it;
	
	SNAKE.Init(MAXNODESNAKE, BEGINNODESNAKE, 0 ,3);
	SNAKE.InitMap(MAPSIZEX, MAPSIZEY, curLEVEL.MaxStuff, curLEVEL.AmountBlock, curLEVEL.PosBlock);
	VECTORHORIZ = 1;
	VECTORVERT = 0;
	
	TotalPoint = 0;
	LevelTime = 0;
	BetweenTime = 0;
	BetweenTimeTmp = 1;

	cSkinSnake = RGB(120, 120, 120);
	SendWorkStatus();
	MyRedrawWindow(WorkWnd, memBit);
	InvalidateRect(WorkWnd, NULL, TRUE);
	UpdateWindow(WorkWnd);
}

void SendWorkStatus()
{
	TCHAR status[100];
	TCHAR dest[10];
	if (run)
		_tcscpy_s(status, L"Game is running | ");
	else
		_tcscpy_s(status, L"Game was stopped | ");
	_tcscat_s(status, L"Current level : ");
	_itot_s(curLEVEL.LevelNum, dest, 10);
	_tcscat_s(status, dest);
	SendMessage(WorkStatusBar, SB_SETTEXT, SBT_NOBORDERS, (LPARAM)status);
}

void DrawLevel(HWND hWnd, HDC &hdc, RECT SnakeRect)
{
	hBrushSnake = CreateSolidBrush(cSkinSnake);
	SelectObject(hdc, hBrushSnake);

	if (isCrashSnake)
	{
		HFONT hFont = CreateFont(28, 0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET,
			0, 0, 0, 0,
			L"Segoe UI");
		SelectObject(hdc, hFont);
		TextOut(hdc, BEGINMAPX + MAPX/2 - 60, BEGINMAPY + MAPY / 2, L"SNAKE WAS CRASED", 17);
		TCHAR str[20];
		_stprintf_s(str, L"YOUR POINT: %.0f", TotalPointCopy);
		TextOut(hdc, BEGINMAPX + MAPX / 2 - 40, BEGINMAPY + MAPY / 2 + 20, str, _tcslen(str));

	}

	for (int i = 0; i < SNAKE.GetCountNode(); i++)
	{
		SetRect(&RGTL[i],
			SnakeRect.left + SNAKE.GetCurNodePosX(i)*MAPSTEP + 2,
			SnakeRect.top + SNAKE.GetCurNodePosY(i)*MAPSTEP + 2,
			SnakeRect.left + (SNAKE.GetCurNodePosX(i) + 1)*MAPSTEP - 2,
			SnakeRect.top + (SNAKE.GetCurNodePosY(i) + 1)*MAPSTEP - 2);
		FillRect(hdc, &RGTL[i], hBrushSnake);
	}

	hGlobalPen = GetStockPen(NULL_PEN);
	SelectObject(hdc, hGlobalPen);
	Rectangle(hdc,
		SnakeRect.left + SNAKE.GetPosStuffX()*MAPSTEP + 2,
		SnakeRect.top + SNAKE.GetPosStuffY()*MAPSTEP + 2,
		SnakeRect.left + (SNAKE.GetPosStuffX() + 1)*MAPSTEP - 2,
		SnakeRect.top + (SNAKE.GetPosStuffY() + 1)*MAPSTEP - 2);

	HBRUSH hBrushBlock = CreateHatchBrush(HS_FDIAGONAL, RGB(200, 200, 200));
	SelectObject(hdc, hBrushBlock);
	for (int j = 0; j < SNAKE.GetAmountBlock(); j++)
	{
		if (curLEVEL.PosBlock[j].x != NOMATTER)
		{
			Rectangle(hdc,
				SnakeRect.left + curLEVEL.PosBlock[j].x*MAPSTEP + 2,
				SnakeRect.top + curLEVEL.PosBlock[j].y*MAPSTEP + 2,
				SnakeRect.left + (curLEVEL.PosBlock[j].x + 1)*MAPSTEP - 2,
				SnakeRect.top + (curLEVEL.PosBlock[j].y + 1)*MAPSTEP - 2);
		}
	}
	DeleteObject(hBrushBlock);
	DeleteObject(hBrushSnake);
}

void CrashSnake()
{

}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	INT iRes;
	TCHAR res[150];

	switch (uMsg)
	{
	case WM_TIMER:
		if (idEvent == 10)
		{
			SNAKE.Run(VECTORHORIZ, VECTORVERT);
			iRes = SNAKE.GetCond();
			if ((iRes == EATITSELF) || (iRes == CRASHRUN))
			{
				isCrashSnake = TRUE;
				AddScores();

				SendMessage(hwnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0);
				LoadLevel();
			}
			if (iRes == EATALLSTUFF)
			{
				MyRedrawWindow(WorkWnd, memBit);
				InvalidateRect(WorkWnd, &pointRect, 0);
				if (curUSER.isAllLevelOpen == 1)
				{
					AddScores();

					SendMessage(hwnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0);
					EnableWindow(WorkStartButton, FALSE);
					isLevelChoosen = FALSE;
					_stprintf_s(res, L"Your results: %.0f points\nChoose any level in settings", TotalPoint);
					MessageBox(NULL, res, L"Level is passed", MB_OK);
				}
				else
				{
					if (curUSER.ProgressLevel >= LastLevel){
						AddScores();
						curUSER.isAllLevelOpen = 1;
						SendMessage(hwnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0);
						_stprintf_s(res, L"Your results: %.0f points\nYou succesfull passed all level\n Level #1 is loading...", TotalPoint);
						MessageBox(NULL, res, L"Congratulation", MB_OK);

						curLEVEL.LevelNum = 1;
						LoadLevel();
					}
					else {
						
						AddScores();

						curUSER.ProgressLevel += 1;
						curLEVEL.LevelNum = curUSER.ProgressLevel;
						SendMessage(hwnd, WM_COMMAND, ID_WORK_STOP_BUTTON, 0);
						_stprintf_s(res, L"Your results: %.0f points\nYou succesfull passed that level\n Level #%d is loading...", TotalPoint, curLEVEL.LevelNum);
						MessageBox(NULL, res, L"Level passed", MB_OK);
						LoadLevel();
					}
				}
			}
			if (iRes == EATSTUFF)
			{
				PlusPoint = TRUE;
				BetweenTimeTmp = BetweenTime;
				TotalPoint += (100 * 1000 / curUSER.SNAKESPEED) + (1000 * SNAKE.GetCountNode()) + (100 / BetweenTimeTmp * 100);
				BetweenTime = 0;
				MyRedrawWindow(WorkWnd, memBit);
				InvalidateRect(WorkWnd, &pointRect, 0);
				InvalidateRect(WorkWnd, &pointRectDown, 0);		
				InvalidateRect(WorkWnd, &pointPlusSpeed, 0);
				InvalidateRect(WorkWnd, &pointPlusSize, 0);
				InvalidateRect(WorkWnd, &pointPlusTime, 0);
				SNAKE.CreatePosStuff();
				
			}
			MyRedrawWindow(WorkWnd, memBit);
			InvalidateRect(WorkWnd, &SnakeRect, 0);
			UpdateWindow(WorkWnd);
			isCrashSnake = FALSE;
			PlusPoint = FALSE;
		}

	}
	
}

void AddScores()
{
	if (TotalPoint == 0) {
		TotalPointCopy = 0;
		return;
	}
	TotalPointCopy = TotalPoint;
	_Table tmpTable;
	tmpTable.LevelNum = curLEVEL.LevelNum;
	_tcscpy_s(tmpTable.UserName, curUSER.UserName);
	tmpTable.Scores = TotalPoint;
	TABLE.push_back(tmpTable);
}

void SaveCurUser()
{
	if (!USER.empty())
	{
		if (curUSER.ProgressLevel)
		{
			std::list<_User>::iterator it;
			it = USER.begin();
			while ((it != USER.end()) && (_tcscmp((*it).UserName, curUSER.UserName))) { it++; }

			(*it).ProgressLevel = curUSER.ProgressLevel;
			(*it).isAllLevelOpen = curUSER.isAllLevelOpen;
			(*it).isExtraSkin = curUSER.isExtraSkin;
			(*it).SKIN = curUSER.SKIN;
			(*it).SNAKESPEED = curUSER.SNAKESPEED;
			for (int i = 0; i < LEVELNUM; i++)
				(*it).records[i] = curUSER.records[i];
			_tcscpy_s((*it).UserName, curUSER.UserName);
		}
	}
	

	if (curUSER.isExtraSkin)
		DeleteFile(L"extra.png");
	pfnSaveUserFile(_T("user.cfg"), &USER, &TABLE);
	pfnSaveUserFile(_T("scores.bin"), &USER, &TABLE);
}

BOOL LoadAllFiles()
{
	DWORD iRes;

	hDLLFunc = LoadLibrary(_T("filedll"));
	if (hDLLFunc)
	{
		pfnSaveUserFile = (PFN)GetProcAddress(hDLLFunc, "SaveUserFile");
		if (pfnSaveUserFile == NULL)
		{
			MessageBox(NULL, _T("Function SaveUserFile not found"),
				_T("LoadLibrary"), MB_OK | MB_ICONQUESTION);
			DestroyWindow(MainWnd);
			return 0;
		}
		pfnLoadUserFile = (PFN2)GetProcAddress(hDLLFunc, "LoadUserFile");
		if (pfnLoadUserFile == NULL)
		{
			MessageBox(NULL, _T("Function LoadUserFile not found"),
				_T("LoadLibrary"), MB_OK | MB_ICONQUESTION);
			DestroyWindow(MainWnd);
			return 0;
		}
		loading++;
		InvalidateRect(MainWnd, &LoadingFile, 1);
		UpdateWindow(MainWnd);
		Sleep(100);
	}
	else
	{
		MessageBox(NULL, _T("filedll.dll not found"), _T("LoadLibrary"),
			MB_OK | MB_ICONQUESTION);
		DestroyWindow(MainWnd);
		return 0;
	}

	iRes = pfnLoadUserFile(_T("user.cfg"), &USER, &LEVEL, &TABLE);
	if (iRes){
		DestroyWindow(MainWnd);
		return FALSE;
	}
	loading++;
	InvalidateRect(MainWnd, &LoadingFile, 1);
	UpdateWindow(MainWnd);
	Sleep(100);

	iRes = pfnLoadUserFile(_T("level.bin"), &USER, &LEVEL, &TABLE);
	if (iRes){
		DestroyWindow(MainWnd);
		return FALSE;
	}
	loading++;
	InvalidateRect(MainWnd, &LoadingFile, 1);
	UpdateWindow(MainWnd);
	Sleep(100);

	iRes = pfnLoadUserFile(_T("addon.dat"), &USER, &LEVEL, &TABLE);
	if (iRes){
		isAddons1 = FALSE;
		loading++;
		DestroyWindow(MainWnd);
		return FALSE;
	}
	else {
		isAddons1 = TRUE;
		loading++;
		InvalidateRect(MainWnd, &LoadingFile, 1);
		UpdateWindow(MainWnd);
		Sleep(100);
	}
		

	iRes = pfnLoadUserFile(_T("addon2.dat"), &USER, &LEVEL, &TABLE);
	if (iRes){
		isAddons2 = FALSE;
		loading++;
		DestroyWindow(MainWnd);
		return FALSE;
	}
	else {
		isAddons2 = TRUE;
		loading++;
		InvalidateRect(MainWnd, &LoadingFile, 1);
		UpdateWindow(MainWnd);
		Sleep(100);
	}
		

	iRes = pfnLoadUserFile(_T("scores.bin"), &USER, &LEVEL, &TABLE);
	if (iRes){
		DestroyWindow(MainWnd);
		return FALSE;
	}
	loading++;
	InvalidateRect(MainWnd, &LoadingFile, 1);
	UpdateWindow(MainWnd);
	Sleep(100);
	loading++;
	InvalidateRect(MainWnd, &LoadingFile, 1);
	UpdateWindow(MainWnd);

	return TRUE;
}
