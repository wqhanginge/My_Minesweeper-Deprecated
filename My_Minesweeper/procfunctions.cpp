#include "procfunctions.h"

bool lparamispos(LPARAM lparam)
{
	POINTS p = MAKEPOINTS(lparam);
	p = { p.x - MAP_LEFT,p.y - MAP_TOP };
	return (p.x >= 0 && p.x < x2rleft(Game.width) && p.y >= 0 && p.y < y2rtop(Game.height));
}

int lparamtoindex(LPARAM lparam)
{
	POINTS p = MAKEPOINTS(lparam);
	return rpos2index(p.x - MAP_LEFT, p.y - MAP_TOP);
}

int str2dword(TCHAR *str, dword len, dword &x)
{
	x = 0;
	for (dword i = 0; i < len && str[i] != TEXT('\0'); i++) {
		if (str[i] < TEXT('0') || str[i] > TEXT('9')) return -1;
		x = x * 10 + str[i] - TEXT('0');
	}
	return 0;
}

int dword2str(TCHAR *str, dword len, dword x)
{
	int ret;
	if (len < 2) return 0;
	ret = (x >= 10) ? dword2str(str, len, x / 10) : 0;
	if ((dword)ret < len - 1) {
		str[ret] = x % 10 + TEXT('0');
		ret++;
	}
	str[ret] = TEXT('\0');
	return ret;
}

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

void paintmap(HDC hdestdc, int muleft, int mutop)
{
	HDC hdcbuffer = CreateCompatibleDC(hdestdc);
	HBITMAP hbmbuffer = CreateCompatibleBitmap(hdestdc, MU_SIZE, MU_SIZE);
	SelectObject(hdcbuffer, hbmbuffer);

	for (word i = 0; i < Game.size; i++) {
		if (!Game.mapchange[i]) continue;
		drawMapUnitNB(hdcbuffer, 0, 0, i);
		BitBlt(hdestdc, muleft + index2rleft(i), mutop + index2rtop(i), MU_SIZE, MU_SIZE, hdcbuffer, 0, 0, SRCCOPY);
	}
	//remove "need update sign"
	memset(Game.mapchange, 0, sizeof(bool)*Game.size);

	DeleteObject(hbmbuffer);
	DeleteObject(hdcbuffer);
}

void paintmapNB(HDC hdestdc, int muleft, int mutop)
{
	for (word i = 0; i < Game.size; i++) {
		if (!Game.mapchange[i]) continue;
		drawMapUnitNB(hdestdc, muleft + index2rleft(i), mutop + index2rtop(i), i);
	}
	//remove "need update sign"
	memset(Game.mapchange, 0, sizeof(bool)*Game.size);
}

/*first unmark clicked units,
 *then refresh whole map,
 *then change clicked units bitmap,
 *finally mark clicked units
 */
void showclickdown(HDC hdestdc, int muleft, int mutop, Neighbor &indexes)
{
	for (word i = 0; i < 9; i++)
		if (indexes[i] >= 0 && indexes[i] < (int)Game.size)
			Game.mapchange[indexes[i]] = false;
	paintmap(hdestdc, muleft, mutop);
	for (word i = 0; i < 9; i++) {
		if (indexes[i] < 0 || indexes[i] >= (int)Game.size) continue;
		if (GETMUSTATE(Game.map[indexes[i]]) == MUS_COVER)
			drawMUUncovBg(hdestdc, muleft + index2rleft(indexes[i]), mutop + index2rtop(indexes[i]));
		else if (GETMUSTATE(Game.map[indexes[i]]) == MUS_MARK)
			drawMUMark(hdestdc, muleft + index2rleft(indexes[i]), mutop + index2rtop(indexes[i]), true);
	}
	for (word i = 0; i < 9; i++)
		if (indexes[i] >= 0 && indexes[i] < (int)Game.size)
			Game.mapchange[indexes[i]] = true;
}

void showclickdown(HDC hdestdc, int muleft, int mutop, int index)
{
	if (index < 0 || index >= (int)Game.size) return;
	Game.mapchange[index] = false;
	paintmap(hdestdc, muleft, mutop);
	if (GETMUSTATE(Game.map[index]) == MUS_COVER)
		drawMUUncovBg(hdestdc, muleft + index2rleft(index), mutop + index2rtop(index));
	else if (GETMUSTATE(Game.map[index]) == MUS_MARK)
		drawMUMark(hdestdc, muleft + index2rleft(index), mutop + index2rtop(index), true);
	Game.mapchange[index] = true;
}

void changechecked(byte GameMode)
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

void changemark(bool Mark)
{
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_STATE;
	mii.fState = (Mark) ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(hMenu, ID_GAME_MARK, FALSE, &mii);
}

int maketimestr(TCHAR * buffer, int size, dword time, const TCHAR * timeunit/*=DEFTIMEUNITEN*/)
{
	int ret;
	if (size < 2) return 0;
	//transform time value into c_string
	ret = dword2str(buffer, size, time);
	if (ret == size - 1) return ret;
	//append time unit c_string
	_tcsncat_s(buffer, size, timeunit, size - ret - 1);
	return (int)_tcslen(buffer);
}

void initgame(TCHAR * Path, POINT &lastwndpos)
{
	lastwndpos.x = GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT("xpos"), DEFWNDLEFT, Path);
	lastwndpos.y = GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT("ypos"), DEFWNDTOP, Path);
	Game.mode = (byte)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT("mode"), CRUSH, Path);
	Game.width = (byte)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT("width"), 0, Path);
	Game.height = (byte)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT("height"), 0, Path);
	Game.mines = (word)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT("mines"), 0, Path);
	Game.mark = (bool)GetPrivateProfileInt(TEXT(INIT_ANAME), TEXT("mark"), 0, Path);
	setGameMode(Game.mode, Game.width, Game.height, Game.mines);
	Score.junior_time = (word)GetPrivateProfileInt(TEXT(SCORE_ANAME), TEXT("timej"), MAX_TIME, Path);
	GetPrivateProfileString(TEXT(SCORE_ANAME), TEXT("namej"), TEXT(DEF_SCORE_NAME_EN), Score.junior_name, SCORE_NAME_LEN, Path);
	Score.middle_time = (word)GetPrivateProfileInt(TEXT(SCORE_ANAME), TEXT("timem"), MAX_TIME, Path);
	GetPrivateProfileString(TEXT(SCORE_ANAME), TEXT("namem"), TEXT(DEF_SCORE_NAME_EN), Score.middle_name, SCORE_NAME_LEN, Path);
	Score.senior_time = (word)GetPrivateProfileInt(TEXT(SCORE_ANAME), TEXT("times"), MAX_TIME, Path);
	GetPrivateProfileString(TEXT(SCORE_ANAME), TEXT("names"), TEXT(DEF_SCORE_NAME_EN), Score.senior_name, SCORE_NAME_LEN, Path);
}

void savegame(TCHAR * Path, POINT &wndpos)
{
	TCHAR str[5];
	dword2str(str, wndpos.x);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT("xpos"), str, Path);
	dword2str(str, wndpos.y);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT("ypos"), str, Path);
	dword2str(str, Game.mode);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT("mode"), str, Path);
	dword2str(str, Game.width);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT("width"), str, Path);
	dword2str(str, Game.height);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT("height"), str, Path);
	dword2str(str, Game.mines);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT("mines"), str, Path);
	dword2str(str, Game.mark);
	WritePrivateProfileString(TEXT(INIT_ANAME), TEXT("mark"), str, Path);
	dword2str(str, Score.junior_time);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT("timej"), str, Path);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT("namej"), Score.junior_name, Path);
	dword2str(str, Score.middle_time);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT("timem"), str, Path);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT("namem"), Score.middle_name, Path);
	dword2str(str, Score.senior_time);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT("times"), str, Path);
	WritePrivateProfileString(TEXT(SCORE_ANAME), TEXT("names"), Score.senior_name, Path);
}

void getversion(TCHAR* version, int size_in_ch)
{
	memset(version, 0, sizeof(TCHAR) * size_in_ch);
	TCHAR szAppFullPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szAppFullPath, MAX_PATH);	//得到程序模块名称，全路径
	
	//获取当前文件的版本信息
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
			//获取文件版本号
			StringCchPrintf(version, size_in_ch, TEXT("%d.%d.%d.%d"),
				HIWORD(pFileInfo->dwFileVersionMS),
				LOWORD(pFileInfo->dwFileVersionMS),
				HIWORD(pFileInfo->dwFileVersionLS),
				LOWORD(pFileInfo->dwFileVersionLS));
		}
		delete pszAppVersion;
	}
}
