#pragma once

/* 
 * this file contains encapsulations of UI operations and Game core operations
 * this file also contians IO functions and Win32 Window Proc Functions
 * NOTE:most functions have no arg check, use with care
 */

#include "stdincludes.h"
#include "gamecore.h"
#include "userinterface.h"


/* Game private window messages */

#define WM_GAMERESET	(WM_APP + 0)		//send when game needs reset
#define WM_GAMESUCCESS	(WM_APP + 1)		//send when game is succeed
#define WM_GAMEFAIL		(WM_APP + 2)		//send when game is failed
#define WM_GAMESTART	(WM_APP + 3)		//send when game needs start, use lparam as start position

 /* send when game mode needs change,
  * use wparam as new GameMode,
  * use lparam as a combination of new width, height and mines
  * ignore lparam if new GameMode is a standard Mode
  * call MAKECHGLAPRAM to create a lparam
  * call GETCHG*** to unpack a lparam
  */
#define WM_GAMEMODECHG	(WM_APP + 4)

//create a lparam by width, height and mines
#define MAKECHGLPARAM(w, h, m)	((LPARAM)((((dword)(w) & 0xFF) | (((dword)(h) & 0xFF) << 8)) | (((dword)(m) & 0xFFFF) << 16)))

//unpack infomation from a lparam
#define GETCHGWIDTH(l)			((byte)((dword)(l) & 0xFF))
#define GETCHGHEIGHT(l)			((byte)(((dword)(l) >> 8) & 0xFF))
#define GETCHGMINES(l)			((word)(((dword)(l) >> 16) & 0xFFFF))


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

/* IO defines and game init defines */

#define DEF_WND_LEFT	128
#define DEF_WND_TOP		128
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



extern HINSTANCE hInst;	//program instance handle
extern HWND hWnd;		//main window handle
extern HMENU hMenu;		//main menu handle

//bitmap handle
extern HBITMAP hbm_rb, hbm_click, hbm_fail, hbm_success;
extern HBITMAP& hbm_current = hbm_rb;


//check if a mouse position is inside the MapUnits area
bool lparamIsInPos(LPARAM laparm);
//change a mouse position to map index
int lparam2index(LPARAM lparam);


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


/* save file management */

//load infomation from a save file
//use default setting to init Game if error
void initGame(TCHAR* Path, POINT &lastwndpos);

//save Game infomation into a save file
void saveGame(TCHAR* Path, POINT &wndpos);


/* get program version information */
void getVersion(TCHAR* version, size_t size_in_ch);



/* window Porc Functions */

//About Dialog, show program description on child modal window
INT_PTR CALLBACK AboutProc(HWND habout, UINT msg, WPARAM wparam, LPARAM lparam);

//Record Dialog, show record on child modal window
INT_PTR CALLBACK RecordProc(HWND hrecord, UINT msg, WPARAM wparam, LPARAM lparam);

//GetName Dialog, provide an edit box to get Record Name after breaking Record
INT_PTR CALLBACK GetNameProc(HWND hgetname, UINT msg, WPARAM wparam, LPARAM lparam);

//Custom Dialog, use to customize Game Map
INT_PTR CALLBACK CustomProc(HWND hcustom, UINT msg, WPARAM wparam, LPARAM lparam);


/* following functions are encapsulations of operations in WndProc */

//response menu message
int onMenu(WPARAM wparam);
