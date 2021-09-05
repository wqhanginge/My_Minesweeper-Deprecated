#include "procfunctions.h"

bool lparamIsInPos(LPARAM lparam)
{
	POINTS p = MAKEPOINTS(lparam);
	p = { p.x - MAP_LEFT,p.y - MAP_TOP };
	return (p.x >= 0 && p.x < x2px(Game.width) && p.y >= 0 && p.y < y2py(Game.height));
}

int lparam2index(LPARAM lparam)
{
	POINTS p = MAKEPOINTS(lparam);
	return ppos2index(p.x - MAP_LEFT, p.y - MAP_TOP);
}
//
//int str2dword(dword& x, TCHAR* str, dword size)
//{
//	x = 0;
//	for (dword i = 0; i < _tcslen(str) && str[i] != TEXT('\0'); i++) {
//		if (str[i] < TEXT('0') || str[i] > TEXT('9')) return -1;
//		x = x * 10 + str[i] - TEXT('0');
//	}
//	return 0;
//}
//
//int dword2str(TCHAR *str, dword len, dword x)
//{
//	StringCchPrintf(str, len, TEXT("%d"), x);
//	return _tcslen(str);
//}

void loadBitmaps()
{
	hbm_rb = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RESET));
	hbm_click = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CLICK));
	hbm_success = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SUCCESS));
	hbm_fail = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FAIL));
}

void freeBitmaps()
{
	DeleteObject(hbm_rb);
	DeleteObject(hbm_click);
	DeleteObject(hbm_success);
	DeleteObject(hbm_fail);
}

void paintMap(HDC hdestdc, int mapleft, int maptop)
{
	HDC hdcbuffer = CreateCompatibleDC(hdestdc);
	HBITMAP hbmbuffer = CreateCompatibleBitmap(hdestdc, MAP_WIDTH, MAP_HEIGHT);
	SelectObject(hdcbuffer, hbmbuffer);
	BitBlt(hdcbuffer, 0, 0, MAP_WIDTH, MAP_HEIGHT, hdestdc, mapleft, maptop, SRCCOPY);
	paintMapNB(hdcbuffer, 0, 0);
	BitBlt(hdestdc, mapleft, maptop, MAP_WIDTH, MAP_TOP, hdcbuffer, 0, 0, SRCCOPY);
	DeleteObject(hbmbuffer);
	DeleteObject(hdcbuffer);
}

void paintMapNB(HDC hdestdc, int mapleft, int maptop)
{
	for (word i = 0; i < Game.size; i++) {
		if (!MUISUPDATE(Game.map[i])) continue;
		drawMapUnitNB(hdestdc, mapleft + index2px(i), maptop + index2py(i), Game.map[i]);
		REMMUUPDATE(Game.map[i]);
	}
}

void paintResetButton(HDC hdestdc, int rbleft, int rbtop, bool clicked)
{
	drawResetButton(hdestdc, rbleft, rbtop, hbm_current, clicked);
}

void setRBBitmap(HBITMAP& hbm)
{
	hbm_current = hbm;
}


/* it needs to clear clicked state after mouse left prior position
 * first remove Update bit of clicked map_units,
 * then refresh the whole map,
 * then redraw clicked map_units,
 * finally set Update bit of clicked map_units
 */

void showClickedMapUnit(HDC hdestdc, int mapleft, int maptop, int index)
{
	if (index < 0 || index >= (int)Game.size) return;
	REMMUUPDATE(Game.map[index]);
	paintMap(hdestdc, mapleft, maptop);
	if (GETMUSTATE(Game.map[index]) == MUS_COVER)
		drawMUUncovBg(hdestdc, mapleft + index2px(index), maptop + index2py(index));
	else if (GETMUSTATE(Game.map[index]) == MUS_MARK)
		drawMUMark(hdestdc, mapleft + index2px(index), maptop + index2py(index), true);
	SETMUUPDATE(Game.map[index]);
}

void showClickedMapUnit(HDC hdestdc, int mapleft, int maptop, Neighbor &indexes)
{
	for (word i = 0; i < 9; i++) {
		if (indexes[i] >= 0 && indexes[i] < (int)Game.size)
			REMMUUPDATE(Game.map[i]);
	}
	paintMap(hdestdc, mapleft, maptop);
	for (word i = 0; i < 9; i++) {
		if (indexes[i] < 0 || indexes[i] >= (int)Game.size) continue;
		if (GETMUSTATE(Game.map[indexes[i]]) == MUS_COVER)
			drawMUUncovBg(hdestdc, mapleft + index2px(indexes[i]), maptop + index2py(indexes[i]));
		else if (GETMUSTATE(Game.map[indexes[i]]) == MUS_MARK)
			drawMUMark(hdestdc, mapleft + index2px(indexes[i]), maptop + index2py(indexes[i]), true);
	}
	for (word i = 0; i < 9; i++) {
		if (indexes[i] >= 0 && indexes[i] < (int)Game.size)
			SETMUUPDATE(Game.map[i]);
	}
}


void setMenuChecked(byte GameMode)
{
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

	switch (GameMode) {
	case JUNIOR:
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_CHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_JUNIOR, FALSE, &mii);
		mii.fState = MFS_UNCHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_MIDDLE, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_SENIOR, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_CUSTOM, FALSE, &mii);
		break;
	case MIDDLE:
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_CHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_MIDDLE, FALSE, &mii);
		mii.fState = MFS_UNCHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_JUNIOR, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_SENIOR, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_CUSTOM, FALSE, &mii);
		break;
	case SENIOR:
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_CHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_SENIOR, FALSE, &mii);
		mii.fState = MFS_UNCHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_JUNIOR, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_MIDDLE, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_CUSTOM, FALSE, &mii);
		break;
	case CUSTOM:
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_CHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_CUSTOM, FALSE, &mii);
		mii.fState = MFS_UNCHECKED;
		SetMenuItemInfo(hMenu, ID_GAME_JUNIOR, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_MIDDLE, FALSE, &mii);
		SetMenuItemInfo(hMenu, ID_GAME_SENIOR, FALSE, &mii);
		break;
	default:
		break;
	}
}

void setQMarkChecked(bool Mark)
{
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_STATE;
	mii.fState = (Mark) ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(hMenu, ID_GAME_MARK, FALSE, &mii);
}
//
//int maketimestr(TCHAR* buffer, int size, dword time, const TCHAR* timeunit/*=DEF_TIMEUNIT_EN*/)
//{
//	if (size < 2) return 0;
//	return (int)_tcslen(buffer);
//}

void initGame(TCHAR* Path, POINT &lastwndpos)
{
	lastwndpos.x = GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT(INIT_XPOS), DEF_WND_LEFT, Path);
	lastwndpos.y = GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT(INIT_YPOS), DEF_WND_TOP, Path);

	int mode, width, height, mines, mark;
	mode = (byte)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT(INIT_MODE), CRUSH, Path);
	width = (byte)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT(INIT_WIDTH), 0, Path);
	height = (byte)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT(INIT_HEIGHT), 0, Path);
	mines = (word)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT(INIT_MINES), 0, Path);
	mark = (bool)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT(INIT_MARK), 0, Path);
	setGameMode(mode, width, height, mines);
	setMark(mark);

	Score.junior_time = (word)GetPrivateProfileInt(TEXT(SCORE_ANAME), TEXT(SCORE_JTIME), MAX_TIME, Path);
	GetPrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_JNAME), TEXT(DEF_SCORE_NAME_EN), Score.junior_name, SCORE_NAME_LEN, Path);
	Score.middle_time = (word)GetPrivateProfileInt(TEXT(SCORE_ANAME), TEXT(SCORE_MTIME), MAX_TIME, Path);
	GetPrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_MNAME), TEXT(DEF_SCORE_NAME_EN), Score.middle_name, SCORE_NAME_LEN, Path);
	Score.senior_time = (word)GetPrivateProfileInt(TEXT(SCORE_ANAME), TEXT(SCORE_STIME), MAX_TIME, Path);
	GetPrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_SNAME), TEXT(DEF_SCORE_NAME_EN), Score.senior_name, SCORE_NAME_LEN, Path);
}

void saveGame(TCHAR* Path, POINT &wndpos)
{
	TCHAR str[CONTENT_STRLEN];
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), wndpos.x);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT(INIT_XPOS), str, Path);
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), wndpos.y);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT(INIT_YPOS), str, Path);

	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Game.mode);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT(INIT_MODE), str, Path);
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Game.width);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT(INIT_WIDTH), str, Path);
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Game.height);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT(INIT_HEIGHT), str, Path);
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Game.mines);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT(INIT_MINES), str, Path);
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Game.mark);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT(INIT_MARK), str, Path);

	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Score.junior_time);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_JTIME), str, Path);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_JNAME), Score.junior_name, Path);
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Score.middle_time);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_MTIME), str, Path);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_MNAME), Score.middle_name, Path);
	StringCchPrintf(str, CONTENT_STRLEN, TEXT("%d"), Score.senior_time);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_STIME), str, Path);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT(SCORE_SNAME), Score.senior_name, Path);
}

void getVersion(TCHAR* version, int size_in_ch)
{
	memset(version, 0, sizeof(TCHAR) * size_in_ch);
	TCHAR szAppFullPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szAppFullPath, MAX_PATH);	//get program module name and full path
	
	//get current exe-file version infomation
	DWORD dwLen = GetFileVersionInfoSize(szAppFullPath, NULL);
	if (dwLen) {
		TCHAR* pszAppVersion = new TCHAR[dwLen + 1];
		memset(pszAppVersion, 0, sizeof(TCHAR) * (dwLen + 1));
		GetFileVersionInfo(szAppFullPath, NULL, dwLen, pszAppVersion);
		UINT nLen = 0;
		VS_FIXEDFILEINFO* pFileInfo = nullptr;
		VerQueryValue(pszAppVersion, TEXT("\\"), (LPVOID*)&pFileInfo, &nLen);
		if (nLen)
		{
			//get file version and print as x.x.x.x form
			StringCchPrintf(version, size_in_ch, TEXT("%d.%d.%d.%d"),
				HIWORD(pFileInfo->dwFileVersionMS),
				LOWORD(pFileInfo->dwFileVersionMS),
				HIWORD(pFileInfo->dwFileVersionLS),
				LOWORD(pFileInfo->dwFileVersionLS));
		}
		delete pszAppVersion;
	}
}
