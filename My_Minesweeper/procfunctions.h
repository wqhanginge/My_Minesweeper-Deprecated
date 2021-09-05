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
#define CONTENT_STRLEN	5
#define INIT_ANAME		"InitInfo"
#define INIT_XPOS		"xpos"
#define INIT_YPOS		"ypos"
#define INIT_MODE		"mode"
#define INIT_WIDTH		"width"
#define INIT_HEIGHT		"height"
#define INIT_MINES		"mines"
#define INIT_MARK		"mark"
#define SCORE_ANAME		"Record"
#define SCORE_JTIME		"junior_time"
#define SCORE_MTIME		"middle_time"
#define SCORE_STIME		"senior_time"
#define SCORE_JNAME		"junior_name"
#define SCORE_MNAME		"middle_name"
#define SCORE_SNAME		"senior_name"


/* APP Infomation defines */

#define APP_NAME		"My Minesweeper"
#define WNDC_NAME		"My_Minesweeper"
#define BUTTON_NAME		"ResetB"
#define ABOUT_INFO_LEN	100
#define ABOUT_TEXT		"My Minesweeper\nversion "

/* miscellaneous defines */

#define GAME_TIMER_ID		1
#define GAME_TIMER_ELAPSE	1000
#define DEF_WND_LEFT		128
#define DEF_WND_TOP			128



extern HBITMAP hbm_rb, hbm_click, hbm_fail, hbm_success;
extern HBITMAP& hbm_current = hbm_rb;


//check if a mouse position is inside the MapUnits area
bool lparamIsInPos(LPARAM laparm);
//change a mouse position to map index
int lparam2index(LPARAM lparam);

//transform between positive integer and c_string
////return -1 if error
//int str2dword(dword& x, TCHAR* str, dword size);
//template <dword size> inline int str2dword(dword& x, TCHAR(&str)[size])
//{ return str2dword(str, size, x); }

////return length of transformed c_string
//int dword2str(TCHAR *str, dword size, dword x);
//template <dword size> inline int dword2str(TCHAR(&str)[size], dword x)
//{ return dword2str(str, size, x); }


//manage bitmaps
void loadBitmaps();
void freeBitmaps();


/* manage MapUnit and ResetButton */

//paint GameMap, the left-top is position 0
//update map_units that have been changed and clear Update bit
//it redraws the whole MapArea content
void paintMap(HDC hdestdc, int mapleft, int maptop);

//paint GameMap, the left-top is position 0
//update map_units that have been changed and clear Update bit
//it will paint directly on DC without creating a DC-Buffer
void paintMapNB(HDC hdestdc, int mapleft, int maptop);

//paint ResetButton without changing its bitmap
void paintResetButton(HDC hdestdc, int rbleft, int rbtop, bool clicked);

//change bitmap of ResetButton
//NOTE:you need to redraw ResetButton after calling this function
void setRBBitmap(HBITMAP& hbm);


//show clicked state when a MapUnit is clicked
//it will do nothing if the index is out of GameMap range
void showClickedMapUnit(HDC hdestdc, int mapleft, int maptop, int index);

//show clicked state when a group of MapUnits are clicked
//it jumps MapUnit which index is out of GameMap range
void showClickedMapUnit(HDC hdestdc, int mapleft, int maptop, Neighbor& indexes);



/* change checked position in Menu */

//change GameMode checked in Menu, do nothing if GameMode is illegal
void setMenuChecked(byte GameMode);

//check Question Mark in Menu if Mark is true
void setQMarkChecked(bool Mark);



////create a c_string to show time score
////return length of the new c_string
//int maketimestr(
//	TCHAR* buffer,
//	int size,
//	dword time,
//	const TCHAR* timeunit = TEXT(DEF_TIMEUNIT_EN)
//);
//template <dword size>
//inline int maketimestr(
//	TCHAR(&buffer)[size],
//	dword time,
//	const TCHAR* timeunit = TEXT(DEF_TIMEUNIT_EN)
//)
//{ return maketimestr(buffer, size, time, timeunit); }


/* save file management */

//load infomation from a save file
//use default setting to init Game if error
void initGame(TCHAR* Path, POINT &lastwndpos);

//save Game infomation into a save file
void saveGame(TCHAR* Path, POINT &wndpos);


/* get program version information */
void getVersion(TCHAR* version, int size_in_ch);
