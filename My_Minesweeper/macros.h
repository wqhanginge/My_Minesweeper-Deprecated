#pragma once

#include <tchar.h>


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned dword;


/* following defines are for user interface
 */


/* following defines for file io
 */

#define DEFFILENAME		"MyMinesweeper.ini"
#define DEFFILEPATHEV	"LOCALAPPDATA"
#define INITAPPNAME		"InitInfo"
#define SCOREAPPNAME	"Record"


/* following defines for miscellaneous
 */

#define GAMETIMERID		1
#define GAMETIMERELAPSE	1000
#define DEFWNDLEFT		128
#define DEFWNDTOP		128
//this use for generate probability, generation function: P = 1 / 2^x, x = mines
//value = 1 << 8
#define MINEPRONE		256


/* following defines for APP Infomation
 */

#define APPNAME			"My Minesweeper"
#define WNDCNAME		"My_Minesweeper"
#define BUTTONNAME		"ResetB"
#define ABOUTINFOLEN	100
#define ABOUTTEXT		"My Minesweeper\nversion "