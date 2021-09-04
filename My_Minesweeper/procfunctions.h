#pragma once

/*
 * this file contains encapsulations of UI operations and Game core operations
 * this file also contians IO functions and Proc Functions for Win32 window
 */

#include "stdincludes.h"
#include "gamecore.h"
#include "userinterface.h"


/* Dialog defines */

//Custom Dialog
#define CUSTOM_EDIT_LEN	8
//end Custom Dialog

//GetName Dialog
#define NAME_EDIT_LEN	SCORE_NAME_LEN
//end GetName Dialog

//Record Dialog
#define TIME_STRLEN		10
#define DEF_TIMEUNIT_EN	" Sec"
#define DEF_TIMEUNIT_CH	" √Î"
//end Record Dialog

/* IO defines */

#define DEF_FILENAME	"MyMinesweeper.ini"
#define DEF_FILEPATH_EV	"LOCALAPPDATA"
#define INIT_ANAME		"InitInfo"
#define SCORE_ANAME		"Record"

/* APP Infomation defines */

#define APPNAME			"My Minesweeper"
#define WNDCNAME		"My_Minesweeper"
#define BUTTONNAME		"ResetB"
#define ABOUTINFOLEN	100
#define ABOUTTEXT		"My Minesweeper\nversion "

/* miscellaneous defines */

#define GAMETIMERID		1
#define GAMETIMERELAPSE	1000
#define DEFWNDLEFT		128
#define DEFWNDTOP		128
 //this use for generate probability, generation function: P = 1 / 2^x, x = mines
 //value = 1 << 8
#define MINEPRONE		256



extern HINSTANCE hInst;	//the program istance handle
extern HWND hWnd;
extern HMENU hMenu;

extern HBITMAP hbm_rb, hbm_click, hbm_fail, hbm_success;


//if a mouse position is inside the MapUnits area
bool lparamispos(_In_ LPARAM laparm);
//change a mouse position to map index
int lparamtoindex(_In_ LPARAM lparam);

//transform between positive integer and c_string
//return -1 if error
int str2dword(
	_In_ TCHAR *str,
	_In_ dword size,
	_Out_ dword &x
);
template <dword size> inline int str2dword(
	_In_ TCHAR(&str)[size],
	_Out_ dword &x
)
{ return str2dword(str, size, x); }

//return length of transformed c_string
int dword2str(
	_Out_ TCHAR *str,
	_In_ dword size,
	_In_ dword x
);
template <dword size> inline int dword2str(
	_Out_ TCHAR(&str)[size],
	_In_ dword x
)
{ return dword2str(str, size, x); }


//manage bitmaps
void loadBitmaps();
void freeBitmaps();

/* manage MapUnit in MapArea */
//paint GameMap, use 'left' and 'top' as position zero of the whole map
void paintmap(
	_In_ HDC hdestdc,
	_In_ int muleft,
	_In_ int mutop
);
//paint GameMap directly, use 'left' and 'top' as position zero of the whole map
void paintmapNB(
	_In_ HDC hdestdc,
	_In_ int muleft,
	_In_ int mutop
);
//paint specific MapUnit directly, use 'left' and 'top' as position zero of the whole map
void showclickdown(
	_In_ HDC hdestdc,
	_In_ int muleft,
	_In_ int mutop,
	_In_ Neighbor &indexes
);
//paint specific MapUnit directly, use 'left' and 'top' as position zero of the whole map
void showclickdown(
	_In_ HDC hdestdc,
	_In_ int muleft,
	_In_ int mutop,
	_In_ int index
);

//change checked position in Menu
void changechecked(_In_ byte GameMode);
void changemark(_In_ bool Mark);

//create a c_string to show time score
//return length of the new c_string
int maketimestr(
	_Out_ TCHAR *buffer,
	_In_ int size,
	_In_ dword time,
	_In_ const TCHAR *timeunit = TEXT(DEF_TIMEUNIT_EN)
);
template <dword size>
inline int maketimestr(
	_Out_ TCHAR(&buffer)[size],
	_In_ dword time,
	_In_ const TCHAR *timeunit = TEXT(DEF_TIMEUNIT_EN)
)
{ return maketimestr(buffer, size, time, timeunit); }


//save file management
void initgame(_In_ TCHAR *Path, _Out_ POINT &lastwndpos);
void savegame(_In_ TCHAR *Path, _In_ POINT &wndpos);


//get file version information
void getversion(_Out_ TCHAR* version, _In_ int size_in_ch);