/***********************************************************************

  UICtrl.h
   NPCOMM UI control definitions.
   Use this with manager->Control(MCC_UI_CTRL,  pUic);
        manager : instance of PCommManager
        UIControl *pUic
   This file is part of NPCOMM transport module interface.
   $Date: 1999/03/21 10:30:48 $ 
   $Revision: 1.2 $

   Copyright (C) 1999 by Hiroyoshi Kurohara
		All Rights Reserved

Permission to use, copy and distribute of this source code
is hereby granted, provided that the above copyright notice appear 
in all copies and no modification is applied to this file..

Hiroyoshi Kurohara disclaims all warranties with regard to this 
software, including all implied warranties of merchantability and 
fitness.
In no event shall Hiroyoshi Kurohara be liable for any special, 
indirect or consequential damages or any damages whatsoever resulting 
from loss of use, data or profits, whether in an action of contract, 
negligence or other tortious action, arising out of or in connection 
with the use or performance of this software.
************************************************************************/
#ifndef _UICTRL_H
#define _UICTRL_H

// control packet to UI
typedef struct UIControlRec {
	int Command;
	int trId;
	void *pData;
} UIControl;

#define UIC_SHEET_ADD			1
#define UIC_SHEET_DEL			2
#define UIC_MENUARRAY_SET		3
#define UIC_MENUARRAY_DELETE	4
#define UIC_TRAY_SETTIP			6
#define UIC_TRAY_CHANGEICON		7
#define UIC_SHEET_CHANGED		8
//
// Dialog sheet definition
typedef struct SheetRec {
	HWND hDlg;
	int trId;
	char title[MAX_PATH];
} Sheet;

// packet definition for EC_MENUITEM_*
#define EX_MS_GRAYED 1
#define EX_MS_CHECKED 2
#define EX_MS_NORMAL 0
typedef struct ExMenuItemRec {
	int idItem;
	int state;
	char DisplayName[MAX_PATH];
	void *pData;
} ExMenuItem;

typedef struct ExMenuArrayRec {
	int trId;
	HWND hWnd;
	int nItems;
	ExMenuItem items[1];
} ExMenuArray;
// transport control message for Ex menu
#define CM_EXMENU_SELECT	(CM_BASE_END + 1)
/////////////////
#define CM_APPLYCHANGE		(CM_EXMENU_SELECT + 1)
#define CM_CANCELCHANGE		(CM_EXMENU_SELECT + 2)
// transport control message for Help
#define CM_SHOWHELP		(CM_CANCELCHANGE + 1)
#define CM_UIC_END		CM_SHOWHELP

// packet definition for UIC_TRAY_*
#define TI_NORMAL 0
#define TI_CONNECT 1
#define TI_BADSTATE 2
typedef struct TrayDataRec {
	int iconKind;
	char *pTipString;
} TrayData;

#endif _UICTRL_H
