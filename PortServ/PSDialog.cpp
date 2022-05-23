/***********************************************************************
    PSDialog.cpp
	User interface part for PortShare server program.

    Copyright (C) 1999-2003 by Hiroyoshi Kurohara
        All Rights Reserved

    This file is part of PortShare server program for Windows.

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
#include <Windows.h>
#include <stdio.h>

#include "PSDialog.h"
#include "Main.h"

extern int gbIsService, gbIsNT;

#define MYWM_NOTIFYICON		(WM_APP+100)
#define IDC_DUMM                        1200
///////////////////////////////////////////////////////////////////

static MessageMap mainMap[] = {
	MESSAGE_HANDLER(PortServDlg, WM_INITDIALOG, InitDialog),
	MESSAGE_HANDLER(PortServDlg, MYWM_NOTIFYICON, OnNotifyIcon),
	MESSAGE_HANDLER(PortServDlg, WM_CLOSE, OnClose),
	COMMAND_HANDLER(PortServDlg, 0, ID_SHOW, SHOW_Menu),
	COMMAND_HANDLER(PortServDlg, 0, ID_EXIT, EXIT_Menu),
	MESSAGE_HANDLER(PortServDlg, WM_QUERYENDSESSION, OnQueryEndSession),
	{ 0, 0, 0, NULL }
} ;

PortServDlg::PortServDlg(HWND parent)
: aDialog(PortServDlg::IDD, parent)
{
	pMesMap = (void *) mainMap;
}

PortServDlg::~PortServDlg()
{
	DestroyMenu(hMenu);

}

void PortServDlg::InitDialog(WPARAM wParam, LPARAM lParam)
{
	SetWindowText(GetDlgItem(IDC_VERSIONSTRING), VERSIONSTRING);
	hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));

	if (bHasSysTray) {
		TrayIconRegister(IDR_MAINFRAME);
		ShowWindow(hDlg, SW_HIDE);
	} else {
		ShowWindow(hDlg, SW_SHOW);
	}
}

void PortServDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	HMENU hPopup = GetSubMenu(hMenu, 0);
	if (lParam == WM_RBUTTONDOWN || lParam == WM_LBUTTONDOWN) {
		POINT pt;

		GetCursorPos(&pt);
		SetActiveWindow(hDlg);
		if (ListBoxGetItemCount(GetDlgItem(IDC_CLIENTLIST)) ||
			gbIsService) {
			EnableMenuItem(hPopup, ID_EXIT,
				MF_BYCOMMAND | MF_GRAYED);
		} else {
			EnableMenuItem(hPopup, ID_EXIT,
				MF_BYCOMMAND | MF_ENABLED);
		}
		//
		TrackPopupMenuEx(hPopup,
			TPM_RIGHTALIGN | TPM_RIGHTBUTTON,
			pt.x, pt.y, hDlg, NULL);
	}
}

void PortServDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
	if (bHasSysTray) {
		ShowWindow(hDlg, SW_HIDE);
	} else {
		EndDialog(hDlg, IDOK);
		EXIT_Menu(0, 0);
	}
}

void PortServDlg::OnQueryEndSession(WPARAM wParam, LPARAM lParam)
{
	if (gbIsService && !gbIsNT && wParam == 0) {
		int procId;
		int (__stdcall *pRegisterServiceProcess) (
			int dwProcessId,   // process identifier
			int dwServiceType  // type of service
			);
		if (gbIsService && !gbIsNT) {
			procId = GetCurrentProcessId();
			pRegisterServiceProcess = 
				(int (__stdcall *)(int, int)) GetProcAddress(
					GetModuleHandle("KERNEL32.DLL"),
					"RegisterServiceProcess");
		}
		if (pRegisterServiceProcess != NULL) {
			(void)(*pRegisterServiceProcess)(procId, 0x00000001);
		}
	}

}

void PortServDlg::SHOW_Menu(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(hDlg, SW_SHOW);
}

void PortServDlg::EXIT_Menu(WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	if (bHasSysTray)
		TrayIconRemove();
}

int PortServDlg::AddConnectionEntry(char *pEntryString)
{
	return ListBoxAddString(GetDlgItem(IDC_CLIENTLIST), pEntryString);
}

int PortServDlg::DeleteConnectionEntry(int iEntry)
{
	return ListBoxDeleteString(GetDlgItem(IDC_CLIENTLIST), iEntry);
}

void PortServDlg::ClearConnectionEntries()
{
	ListBoxResetContents(GetDlgItem(IDC_CLIENTLIST));
}

int PortServDlg::TrayMessage(DWORD dwMessage, UINT uFlags, UINT uID, HICON hIcon, PSTR pszTip)
{
	int res;

	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= hDlg;
	tnd.uID			= uID;

	tnd.uFlags		= uFlags;
	tnd.uCallbackMessage	= MYWM_NOTIFYICON;
	tnd.hIcon		= hIcon;
	if (pszTip) {
		lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip));
	} else {
		tnd.szTip[0] = '\0';
	}

	res = Shell_NotifyIcon(dwMessage, &tnd);


	return res;
}

int PortServDlg::SetTrayTip(char *pStr)
{
	return TrayMessage(NIM_MODIFY, 
				NIF_MESSAGE|NIF_TIP, 
				IDC_DUMM, NULL, pStr);
}

int PortServDlg::SetTrayIcon(int iid)
{
	HICON hIcon;
	int res = FALSE;

	hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(iid));
	if (hIcon) {
		res = TrayMessage(NIM_MODIFY, 
					NIF_MESSAGE|NIF_ICON, 
					IDC_DUMM, hIcon, NULL);
	    DestroyIcon(hIcon);
	}
	return res;
}

int PortServDlg::TrayIconRegister(int iid)
{
	HICON hIcon;
	int res = FALSE;
	hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(iid));
	if (hIcon) {
		res = TrayMessage(NIM_ADD, 
					NIF_MESSAGE|NIF_ICON, 
					IDC_DUMM, hIcon, NULL);
	    DestroyIcon(hIcon);
	}
	return res;
}

int PortServDlg::TrayIconRemove()
{
	return TrayMessage(NIM_DELETE,
				NIF_MESSAGE|NIF_ICON|NIF_TIP,
				IDC_DUMM, NULL, NULL);
}

