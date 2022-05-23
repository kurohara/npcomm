/***********************************************************************
    DlgClass.h
    Simple class for Dialog box UI without MFC.

	Copyright (C) 1999-2003 by Hiroyoshi Kurohara
		All Rights Reserved

  	This file is part of XP module(transport DLL for NPCOMM).
************************************************************************/

/*
   $Date: $ 
   $Revision: $
 */
/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of either
        a) the GNU General Public License as published by the Free Software
           Foundation; either version 2 of the License, or (at your option)
           any later version.
        b) the "Artistic License" .

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License or Artistic License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    You should also have received a copy of the "Artistic License" 
    (which is the file named "Artistic") with this bunch of source codes.
    if not, you can request it to copyright holder of this program.

 */
/*
 This is not a part of NPCOMM transport interfaces.
 So you don't need to use this.
 You may like MFC.
 */
#ifndef _DLGCLASS_H
#define _DLGCLASS_H

class aDialog
{
	HWND hWndParent;
public:
	int iDD;
	HWND hDlg;
	void *pMesMap;
	aDialog();
	aDialog(int id, HWND hWndParent);
	~aDialog();
public:
	virtual int OnCommand(WPARAM wParam, LPARAM lParam);
	HWND GetDlgItem(int id);
	int DoModal(HINSTANCE  hInstance);
	int Create(HINSTANCE  hInstance);

};

typedef struct MessageMapRec {
	UINT message;
	WORD n_kind;
	UINT cid;
	void (aDialog::*handler)(WPARAM, LPARAM);
} MessageMap;

#define DIALOG_HANDLER(SELF, HANDLER) \
( (void (aDialog::*)(WPARAM, LPARAM)) SELF::HANDLER)

#define MESSAGE_HANDLER(SELF, MESSAGE, HANDLER) \
{ MESSAGE, 0, 0, ( (void (aDialog::*)(WPARAM, LPARAM)) SELF::HANDLER) }

#define COMMAND_HANDLER(SELF, KIND, IDC, HANDLER) \
{ WM_COMMAND, KIND, IDC, ( (void (aDialog::*)(WPARAM, LPARAM)) SELF::HANDLER) }


#define ComboBoxAddString(hCB, pString) \
	SendMessage(hCB, CB_ADDSTRING, (WPARAM) 0, (LPARAM) pString)
#define ComboBoxResetContent(hCB) \
	SendMessage(hCB, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0)
#define ComboBoxGetEditText(hCB, pStr, maxLen) \
	GetWindowText(hCB, pStr, maxLen)
#define ComboBoxGetLBText(hCB, index, pStrBuf) \
	SendMessage(hCB, CB_GETLBTEXT, (WPARAM) index, (LPARAM) pStrBuf)
#define ComboBoxSetCurSel(hCB, index) \
	SendMessage(hCB, CB_SETCURSEL, (WPARAM) index, (LPARAM) 0)
#define ComboBoxGetCurSel(hCB) \
	SendMessage(hCB, CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0)
#define ComboBoxGetCount(hCB) \
	SendMessage(hCB, CB_GETCOUNT, (WPARAM) 0, (LPARAM) 0)
#define ComboBoxFindString(hCB, pString) \
	SendMessage(hCB, CB_FINDSTRING, (WPARAM) -1, (LPARAM) pString)
#define ButtonSetCheck(hBt, fCheck) \
	SendMessage(hBt, BM_SETCHECK, (WPARAM) fCheck, (LPARAM) 0)
#define ButtonGetCheck(hBt) \
	SendMessage(hBt, BM_GETCHECK, (WPARAM) 0, (LPARAM) 0)


#endif _DLGCLASS_H
