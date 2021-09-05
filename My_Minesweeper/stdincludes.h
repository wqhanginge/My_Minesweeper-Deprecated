#pragma once

#define WIN32_LEAN_AND_MEAN
#define UNICODE

#include <Windows.h>
#include <tchar.h>
#include <ctime>
//#include <cstdlib>
#include <memory>
#include "resource.h"
#include <strsafe.h>
#pragma comment(lib, "Version.lib")
using namespace std;


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned dword;


extern HINSTANCE hInst;	//the program istance handle
extern HWND hWnd;
extern HMENU hMenu;
