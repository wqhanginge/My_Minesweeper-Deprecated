#include "stdincludes.h"
#include "gamecore.h"
#include "userinterface.h"
#include "procfunctions.h"

#define ID_RESETB	50000

HINSTANCE hInst;
HWND hWnd;
HMENU hMenu;


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static HWND hresetb;
	//use for paint
	static HDC hdc;
	HDC hpaintdc, hdcbuffer;
	PAINTSTRUCT ps;
	HBITMAP hbmbuffer;
	//use for file access
	static TCHAR buffer[MAX_PATH];
	POINT wndpos;
	//use for changing window size
	static int edgew, edgeh;
	RECT wndrect, cltrect;
	//use for GameMap updating
	int index;
	Neighbor indexes;
	//remember if last was double mouse button down
	static bool lastdoublemb;

	switch (msg) {
	case WM_CREATE:
		//load bitmaps
		loadBitmaps();
		//get paint device contex
		hdc = GetDC(hwnd);

		//init game from file
		GetEnvironmentVariable(TEXT(DEF_FILEPATH_EV), buffer, MAX_PATH);
		_tcscat_s(buffer, TEXT("\\"));
		_tcscat_s(buffer, TEXT(DEF_FILENAME));
		initGame(buffer, wndpos);
		simlogistic((dword)time(nullptr));

		//init menu info
		setMenuChecked(Game.mode);
		setQMarkChecked(Game.mark);

		//create reset button
		hresetb = CreateWindowEx(0, TEXT("BUTTON"), TEXT(BUTTONNAME),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			RB_LEFT, RESETBTOP,
			RESETBSIZE, RESETBSIZE,
			hwnd,
			(HMENU)ID_RESETB,
			hInst,
			NULL);
		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_rb);

		//adjust window size
		GetWindowRect(hwnd, &wndrect);
		GetClientRect(hwnd, &cltrect);
		edgew = (wndrect.right - wndrect.left) - cltrect.right;
		edgeh = (wndrect.bottom - wndrect.top) - cltrect.bottom;
		wndrect = { wndpos.x,wndpos.y,wndpos.x + CLIENT_WIDTH + edgew,wndpos.y + CLIENT_HEIGHT + edgeh };
		MoveWindow(hwnd, wndrect.left, wndrect.top, wndrect.right - wndrect.left, wndrect.bottom - wndrect.top, FALSE);

		//init double mouse button state
		lastdoublemb = false;
		break;
	case WM_DESTROY:
		//release bitmaps
		freeBitmaps();
		//release device contex
		ReleaseDC(hwnd, hdc);
		//save game info
		GetWindowRect(hwnd, &wndrect);
		wndpos = { wndrect.left,wndrect.top };
		saveGame(buffer, wndpos);

		PostQuitMessage(0);
		break;
	case WM_PAINT:
		//redraw the whole client area
		hpaintdc = BeginPaint(hwnd, &ps);
		hdcbuffer = CreateCompatibleDC(hpaintdc);
		hbmbuffer = CreateCompatibleBitmap(hpaintdc, CLIENT_WIDTH, CLIENT_HEIGHT);
		SelectObject(hdcbuffer, hbmbuffer);

		drawClientBg(hdcbuffer, 0, 0);
		drawInfoBg(hdcbuffer, INFO_LEFT, INFO_TOP);
		drawMapAreaBg(hdcbuffer, MAPAREALEFT, MAPAREATOP);
		drawMineBg(hdcbuffer, MINELEFT, MINETOP);
		drawTimeBg(hdcbuffer, TIME_LEFT, TIMETOP);
		drawINNB(hdcbuffer, IMNSLEFT, IMNSTOP, Game.mine_remains);
		drawINNB(hdcbuffer, TIMENUMS_LEFT, ITNSTOP, Game.time);
		memset(Game.mapchange, 1, sizeof(bool)*Game.size);
		paintMapNB(hdcbuffer, MAPUNITSLEFT, MAPUNITSTOP);

		BitBlt(hpaintdc, 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, hdcbuffer, 0, 0, SRCCOPY);
		DeleteObject(hbmbuffer);
		DeleteObject(hdcbuffer);

		EndPaint(hwnd, &ps);
		break;
	case WM_COMMAND:
		if (lparam == 0 && HIWORD(wparam) == 0) onMenu(wparam);
		else if (LOWORD(wparam) == ID_RESETB) {
			if (HIWORD(wparam) == BN_CLICKED) {
				PostMessage(hwnd, WM_GAMERESET, 0, 0);
				SetFocus(hwnd);
			}
		}
		break;
	case WM_GAMERESET:
		KillTimer(hwnd, GAMETIMERID);
		resetGame();
		lastdoublemb = false;
		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_rb);
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_GAMESTART:
		createGameMap((int)lparam);
		Game.state = PROGRESS;
		Game.time = 0;
		SetTimer(hwnd, GAMETIMERID, GAMETIMERELAPSE, NULL);
		clickUnit((int)lparam);
		openBlanks((int)lparam);
		if (Game.mines == Game.size - Game.uncov_units)
			PostMessage(hwnd, WM_GAMESUCCESS, 0, 0);
		else
			paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
		break;
	case WM_GAMEFAIL:
		KillTimer(hwnd, GAMETIMERID);
		Game.state = FAIL;
		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_fail);
		//show all mines positions when game set
		uncovAllMines();
		paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
		break;
	case WM_GAMESUCCESS:
		KillTimer(hwnd, GAMETIMERID);
		Game.state = SUCCESS;
		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_success);
		//show all mines positions when game set
		Game.mine_remains = 0;
		drawIN(hdc, IMNSLEFT, IMNSTOP, 0);
		uncovAllMines();
		paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
		//if break record
		if (!ISCRUSH(Game.mode) && Game.time < getRecordTime()) {
			setRecordTime();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_GETNAME), hwnd, GetNameProc);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_RECORD), hwnd, RecordProc);
		}
		break;
	case WM_GAMEMODECHG:
		KillTimer(hwnd, GAMETIMERID);
		setGameMode((byte)wparam, GETCHGWIDTH(lparam), GETCHGHEIGHT(lparam), GETCHGMINES(lparam));
		lastdoublemb = false;
		//change window size for new game map size, no need to reset
		GetWindowRect(hwnd, &wndrect);
		wndrect.right = CLIENT_WIDTH + edgew + wndrect.left;
		wndrect.bottom = CLIENT_HEIGHT + edgeh + wndrect.top;
		//invalidate all client area for repaint
		InvalidateRect(hwnd, NULL, FALSE);
		MoveWindow(hwnd, wndrect.left, wndrect.top, wndrect.right - wndrect.left, wndrect.bottom - wndrect.top, TRUE);
		MoveWindow(hresetb, RB_LEFT, RESETBTOP, RESETBSIZE, RESETBSIZE, TRUE);
		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_rb);
		break;
	case WM_LBUTTONDOWN:
		//won't work after game finishing
		if (Game.state == FAIL || Game.state == SUCCESS) break;

		SetCapture(hwnd);
		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_click);

		//break when not in the map area
		if (!lparamIsInPos(lparam)) break;
		index = lparam2index(lparam);
		if (wparam & MK_RBUTTON) {	//double buttons down
			lastdoublemb = true;
			getNeighbors(indexes, index);
			showClickedMapUnit(hdc, MAPUNITSLEFT, MAPUNITSTOP, indexes);
		}
		else {	//single button
			lastdoublemb = false;
			showClickedMapUnit(hdc, MAPUNITSLEFT, MAPUNITSTOP, index);
		}
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		//won't work after game finishing
		if (Game.state == FAIL || Game.state == SUCCESS) break;

		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_rb);

		//break when not in the map area
		if (!lparamIsInPos(lparam)) break;
		index = lparam2index(lparam);
		if (wparam & MK_RBUTTON) {	//double buttons
			lastdoublemb = true;
			int ret = digNeighbors(index);
			if (ret == -1) {
				PostMessage(hwnd, WM_GAMEFAIL, 0, 0);
			}
			else if (Game.mines == Game.size - Game.uncov_units) {
				PostMessage(hwnd, WM_GAMESUCCESS, 0, 0);
			}
			else
				paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
		}
		else if (!lastdoublemb) {	//single button and last was not double button
			lastdoublemb = false;
			if (Game.state == INIT && GETMUSTATE(Game.map[index]) != MUS_FLAG) {	//first click
				PostMessage(hwnd, WM_GAMESTART, 0, index);
			}
			else {	//not first click
				int ret = clickUnit(index);
				if (ret == 0) openBlanks(index);
				if (ret == -1) {
					PostMessage(hwnd, WM_GAMEFAIL, 0, 0);
				}
				else if (Game.mines == Game.size - Game.uncov_units) {
					PostMessage(hwnd, WM_GAMESUCCESS, 0, 0);
				}
				else
					paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
			}
		}
		else {	//single button and last was double button
			lastdoublemb = false;
			paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
		}
		break;
	case WM_RBUTTONDOWN:
		//won't work after game finishing
		if (Game.state == FAIL || Game.state == SUCCESS) break;

		//break when not in the map area
		if (!lparamIsInPos(lparam)) break;
		index = lparam2index(lparam);
		if (wparam & MK_LBUTTON) {	//double buttons
			lastdoublemb = true;
			PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_click);
			getNeighbors(indexes, index);
			showClickedMapUnit(hdc, MAPUNITSLEFT, MAPUNITSTOP, indexes);
		}
		else {	//single button, flag a unit or mark a unit
			lastdoublemb = false;
			if (GETMUSTATE(Game.map[index]) == MUS_COVER) {
				SETMUSTATE(MUS_FLAG, Game.map[index]);
				drawMapUnit(hdc, MAPUNITSLEFT + index2px(index), MAPUNITSTOP + index2py(index), index);
				Game.mine_remains--;
				drawIN(hdc, IMNSLEFT, IMNSTOP, Game.mine_remains);
			}
			else if (GETMUSTATE(Game.map[index]) == MUS_FLAG && Game.mark) {
				SETMUSTATE(MUS_MARK, Game.map[index]);
				drawMapUnit(hdc, MAPUNITSLEFT + index2px(index), MAPUNITSTOP + index2py(index), index);
				Game.mine_remains++;
				drawIN(hdc, IMNSLEFT, IMNSTOP, Game.mine_remains);
			}
			else if (GETMUSTATE(Game.map[index]) == MUS_FLAG && !Game.mark)
			{
				SETMUSTATE(MUS_COVER, Game.map[index]);
				drawMapUnit(hdc, MAPUNITSLEFT + index2px(index), MAPUNITSTOP + index2py(index), index);
				Game.mine_remains++;
				drawIN(hdc, IMNSLEFT, IMNSTOP, Game.mine_remains);
			}
			else if (GETMUSTATE(Game.map[index]) == MUS_MARK) {
				SETMUSTATE(MUS_COVER, Game.map[index]);
				drawMapUnit(hdc, MAPUNITSLEFT + index2px(index), MAPUNITSTOP + index2py(index), index);
			}
		}
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		//won't work after game finishing
		if (Game.state == FAIL || Game.state == SUCCESS) break;

		PostMessage(hresetb, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm_rb);

		//break when not in the map area
		if (!lparamIsInPos(lparam)) break;
		index = lparam2index(lparam);
		if (wparam & MK_LBUTTON) {	//double buttons
			lastdoublemb = true;
			int ret = digNeighbors(index);
			if (ret == -1) {
				PostMessage(hwnd, WM_GAMEFAIL, 0, 0);
			}
			else if (Game.mines == Game.size - Game.uncov_units) {
				PostMessage(hwnd, WM_GAMESUCCESS, 0, 0);
			}
			else
				paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
		}
		else	//single button
			lastdoublemb = false;
		break;
	case WM_MOUSEMOVE:
		SetFocus(hwnd);
		//won't work after game finishing
		if (Game.state == FAIL || Game.state == SUCCESS) break;

		//break when not in the map area
		if (!lparamIsInPos(lparam)) {
			paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
			break;
		}
		index = lparam2index(lparam);
		if (wparam & MK_LBUTTON) {	//with left button down
			if (wparam & MK_RBUTTON) {	//double buttons
				Neighbor indexes;
				getNeighbors(indexes, index);
				showClickedMapUnit(hdc, MAPUNITSLEFT, MAPUNITSTOP, indexes);
			}
			//single button
			else
				showClickedMapUnit(hdc, MAPUNITSLEFT, MAPUNITSTOP, index);
		}
		//without mouse button down
		else
			paintMap(hdc, MAPUNITSLEFT, MAPUNITSTOP);
		break;
	case WM_TIMER:
		if (Game.time <= MAX_TIME) {
			Game.time++;
			drawIN(hdc, TIMENUMS_LEFT, ITNSTOP, Game.time);
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wndc = { sizeof(WNDCLASSEX) };
	wndc.cbClsExtra = 0;
	wndc.cbWndExtra = 0;
	wndc.hInstance = hInstance;
	wndc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndc.lpfnWndProc = WndProc;
	wndc.lpszClassName = TEXT(WNDCNAME);
	wndc.lpszMenuName = NULL;
	wndc.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wndc);

	hInst = hInstance;
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	hWnd = CreateWindowEx(0, wndc.lpszClassName, TEXT(APPNAME),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		hMenu,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, hWnd, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}