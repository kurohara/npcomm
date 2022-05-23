/* DialogUI.h */
/*
   $Date: $ 
   $Revision: $
 */
/*
    Copyright (C) 1999-2003 Hiroyoshi Kurohara all rights reserved.
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
#ifndef _DIALOGUI_H
#define _DIALOGUI_H

#define MYWM_NOTIFYICON		(WM_APP+100)
#define MYWM_DEFERWORK		(WM_APP+101)
#define ID_MODEMENU_START  (ID_MENUITEM32775 + 100)
#define ID_EXMENU_START	(ID_MODEMENU_START + 100)
#define MODEMENUPOS		5
#define EXMENUPOS		2

///////////////////////////
// for DDE
#define SERVICENAME "NPCOMM"
#define TOPIC_SYSTEM "System"
// DDE execute command bases
// for SYSTEM topic
#define SETFUNCTIONMODE "SetFunctionMode"
#define RELOADSETTING "ReloadSetting"
// for REQUEST transaction
// item
#define RESULT_ITEM "RESULT_STRING"
//

#include "DlgClass.h"
#include "pcommxs.h"
#include "UICtrl.h"
#include "V2Mgr.h"

class MainFrame : public aDialog
{
	BOOL TrayMessage(DWORD dwMessage, UINT uFlags, UINT uID, HICON hIcon, PSTR pszTip);
	// for dialog sheets
	Sheet Sheets[MAX_TRANSPORTS];
	int nSheets;
	ExMenuArray *pMenuArray;
	// data for DDE
	PCommTransport *XsDDEConnected;
	int bDDEConnected;
	ATOM	aAppl;
	ATOM	aSystemTopic;
	ATOM	trTopics[MAX_TRANSPORTS];
	// for transport select 
	int nTransports;
	int trIds[MAX_TRANSPORTS];
	char *pTrNames[MAX_TRANSPORTS];
	//
	int curIcon;
public:
	enum { IDD = IDD_SHEETFRAME} ;
	MainFrame(HWND parent = NULL);
	~MainFrame();
	// V2Manager also has pointer to me.
	// I dislike this kind of cross reference.
	V2Manager *pMgr;

	virtual int OnCommand(WPARAM wParam, LPARAM lParam);

	void InitDialog(WPARAM wParam, LPARAM lParam);
	void OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	void OnClose(WPARAM wParam, LPARAM lParam);
	void OnHelp(WPARAM wParam, LPARAM lParam);
	void OnDefer(WPARAM wParam, LPARAM lParam);
	void OK_Clicked(WPARAM wParam, LPARAM lParam);
	void CANCEL_Clicked(WPARAM wParam, LPARAM lParam);
	void APPLY_Clicked(WPARAM wParam, LPARAM lParam);
	void LOCAL_PORT_Clicked(WPARAM wParam, LPARAM lParam);
	void SHOW_Menu(WPARAM wParam, LPARAM lParam);
	void HELP_Menu(WPARAM wParam, LPARAM lParam);
	void EXIT_Menu(WPARAM wParam, LPARAM lParam);
	void Mode_Changed(WPARAM wParam, LPARAM lParam);
	//
	void OnDDEInitiate(WPARAM wParam, LPARAM lParam);
	void OnDDEExecute(WPARAM wParam, LPARAM lParam);
	void OnDDETerminate(WPARAM wParam, LPARAM lParam);
	void OnDDERequest(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnQueryEndSession(WPARAM wParam, LPARAM lParam);
	void OnUserChanged(WPARAM wParam, LPARAM lParam);
	//

	void ShowPopupMenu();
	void Show(BOOL bShow);
	void EnableApplyButton(BOOL bEnable);
	BOOL SetTrayTip(char *pStr);
	BOOL SetTrayIcon(int iid);
	BOOL TrayIconRegister(int iid);
	BOOL TrayIconRemove(void);
	void SetTrayIconDefer(int iid);
	void SetTrayTipDefer(char *pTip);
	int ChangeTransport(int index);
	//
	int AddXPMode(int trId, char *pName);
	int DeleteXPMode(int trId);
	void SetInitialXPMode(int trId);
	//
	int AddSheet(Sheet *pSheet);
	int ShowSheet(int idSheet);

	//
	int Control(int Command, void *pCd);
	//
	void ProcessDDE(XS_DDE *pPack);
} ;

class AboutSheet : public aDialog
{
public:
	enum { IDD = IDD_ABOUTBOX} ;
	AboutSheet(HWND parent = NULL);
	~AboutSheet();
	V2Manager *pMgr;

	void InitDialog(WPARAM wParam, LPARAM lParam);
	//
} ;

class ShutDialog : public aDialog
{
public:
	enum { IDD = IDD_SHUTDOWN } ;
	ShutDialog(HWND parent = NULL);
	~ShutDialog();

	void InitDialog(WPARAM wParam, LPARAM lParam);
} ;

class DDEforService : public aDialog
{
public:
};
#endif _DIALOGUI_H

