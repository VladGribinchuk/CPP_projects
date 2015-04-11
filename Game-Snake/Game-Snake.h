#pragma once

#ifdef UNICODE
#define _UNICODE
#endif

#include "resource.h"
#include <Windows.h>
#include <Windowsx.h>
#include <commdlg.h>
#include <commctrl.h>
#include <process.h>
#include "ShellAPI.h"
#include <objidl.h>
#include <gdiplus.h>
#include "Snake.h"
#include "icon.h"

// neccessary library for gdi+
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "ComCtl32.lib")

//Users ID command
#define ID_BEGIN_BUTTON	101
#define ID_CLK_LISTBOX	102
#define ID_START_BUTTON 111
#define ID_WORK_START_BUTTON 113
#define ID_WORK_STOP_BUTTON 107
#define ID_WORK_CLOSE 106
#define ID_SETTING_OPEN 110
#define ID_SKIN_SNAKE 112
#define ID_SCORES_TABLE 114

#define IDI_ICON1        152
#define IDI_SNAKEICON    152
