/***********************************************************************
    PSDialog.h
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
#include "DlgClass.h"

#define MYWM_NOTIFYICON		(WM_APP+100)
#define MYWM_DEFERWORK		(WM_APP+101)
#include "resource.h"

class PortServDlg : public aDialog
{
public:
	PortServDlg(HWND parent = NULL);
	~PortServDlg();
	enum { IDD = IDD_PORTSERV_DIALOG };
	int bHasSysTray;
	HMENU hMenu;
// Command handlers
	void InitDialog(WPARAM wParam, LPARAM lParam);
	void OnClose(WPARAM wParam, LPARAM lParam);
	void OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	void SHOW_Menu(WPARAM wParam, LPARAM lParam);
	void EXIT_Menu(WPARAM wParam, LPARAM lParam);
	void OnQueryEndSession(WPARAM wParam, LPARAM lParam);
	//
	void ClearConnectionEntries();
	int AddConnectionEntry(char *pEntryString);
	int DeleteConnectionEntry(int iEntry);
private:
	int TrayMessage(DWORD dwMessage, UINT uFlags, UINT uID, HICON hIcon, PSTR pszTip);
	int SetTrayTip(char *pStr);
	int SetTrayIcon(int iid);
	int TrayIconRegister(int iid);
	int TrayIconRemove();

};

