/* DialogUI.cpp */
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

#include <Windows.h>
#include <Afxres.h>
#include <process.h>
#include "resource.h"
#include <dde.h>
#include <COMMCTRL.h>
#include <stdio.h>

#include "DialogUI.h"
#include "V2Mgr.h"
#include "UICtrl.h"

extern int AppMessageBox(unsigned int resid, unsigned int mask);
extern void GetUserStation(int bGet);

static MessageMap myMap[] = {
	MESSAGE_HANDLER(MainFrame, WM_INITDIALOG, InitDialog),
	MESSAGE_HANDLER(MainFrame, WM_CLOSE, OnClose),
	MESSAGE_HANDLER(MainFrame, MYWM_NOTIFYICON, OnNotifyIcon),
	MESSAGE_HANDLER(MainFrame, WM_HELP, OnHelp),
	MESSAGE_HANDLER(MainFrame, MYWM_DEFERWORK, OnDefer),
	COMMAND_HANDLER(MainFrame, BN_CLICKED, IDC_OK, OK_Clicked),
	COMMAND_HANDLER(MainFrame, BN_CLICKED, IDC_CANCEL, CANCEL_Clicked),
	COMMAND_HANDLER(MainFrame, BN_CLICKED, IDC_APPLY, APPLY_Clicked),
	COMMAND_HANDLER(MainFrame, BN_CLICKED, IDC_LOCAL_PORT, LOCAL_PORT_Clicked),
	COMMAND_HANDLER(MainFrame, 0, ID_SHOW, SHOW_Menu),
	COMMAND_HANDLER(MainFrame, 0, ID_HELP, HELP_Menu),
	COMMAND_HANDLER(MainFrame, 0, ID_EXIT, EXIT_Menu),
	COMMAND_HANDLER(MainFrame, CBN_SELCHANGE, IDC_SELMODE, Mode_Changed),
	// message handling for standard DDE commands
	MESSAGE_HANDLER(MainFrame, WM_DDE_INITIATE, OnDDEInitiate),
	MESSAGE_HANDLER(MainFrame, WM_DDE_EXECUTE, OnDDEExecute),
	MESSAGE_HANDLER(MainFrame, WM_DDE_TERMINATE, OnDDETerminate),
	MESSAGE_HANDLER(MainFrame, WM_DDE_REQUEST, OnDDERequest),
	MESSAGE_HANDLER(MainFrame, WM_NOTIFY, OnNotify),
	//
	MESSAGE_HANDLER(MainFrame, WM_QUERYENDSESSION, OnQueryEndSession),
	MESSAGE_HANDLER(MainFrame, WM_USERCHANGED, OnUserChanged),

	{ 0, 0, 0, NULL }
} ;

static MessageMap aboutMap[] = {
	MESSAGE_HANDLER(AboutSheet, WM_INITDIALOG, InitDialog),
	{ 0, 0, 0, NULL }
} ;

static MessageMap shutMap[] = {
	MESSAGE_HANDLER(ShutDialog, WM_INITDIALOG, InitDialog),
	{ 0, 0, 0, NULL }
} ;
/// shutdown dialog
ShutDialog::ShutDialog(HWND parent)
: aDialog(ShutDialog::IDD, parent)
{
	pMesMap = (void *) shutMap;
}

ShutDialog::~ShutDialog()
{
}

void ShutDialog::InitDialog(WPARAM wParam, LPARAM lParam)
{
}

// About Sheet implementation
AboutSheet::AboutSheet(HWND parent)
: aDialog(AboutSheet::IDD, parent)
{
	pMesMap = (void *) aboutMap;
}

AboutSheet::~AboutSheet()
{
}

void AboutSheet::InitDialog(WPARAM wParam, LPARAM lParam)
{
	SetWindowText(GetDlgItem(IDC_APPVERSION), APPVERSION);

}

// Main dialog frame implementation
MainFrame::MainFrame(HWND parent)
: aDialog(MainFrame::IDD, parent)
{
	pMesMap = (void *) myMap;
	nSheets = 0;
	nTransports = 0;
	pMenuArray = NULL;

	int i;
	for (i=0;i<MAX_TRANSPORTS;i++)
		trTopics[i] = NULL;

	curIcon = -1;

	bDDEConnected = FALSE;
}

MainFrame::~MainFrame()
{
	DeleteAtom(aAppl);
	DeleteAtom(aSystemTopic);
	int i;
	for (i=0;i<MAX_TRANSPORTS;i++) {
		if (trTopics[i])
			DeleteAtom(trTopics[i]);
	}
}

int MainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) >= ID_MODEMENU_START &&
		LOWORD(wParam) < ID_MODEMENU_START + 100) {
		int index;
		int rval;
		index = LOWORD(wParam) - ID_MODEMENU_START;
		rval = ChangeTransport(index);
		if (rval)
			ComboBoxSetCurSel(GetDlgItem(IDC_SELMODE), index);
		return TRUE;
	}
	if (LOWORD(wParam) >= ID_EXMENU_START &&
		LOWORD(wParam) < ID_EXMENU_START + 100) {
		if (pMenuArray == NULL || 
			pMenuArray->nItems == 0)
			return TRUE;
		PCommTransport *pTransport = pMgr->GetTransportById(pMenuArray->trId);
		if (pTransport) pTransport->Control(CM_EXMENU_SELECT, 
			(void *) (LOWORD(wParam) - ID_EXMENU_START) );

		return TRUE;
	}
	aDialog::OnCommand(wParam, lParam);
	return TRUE;
}

extern int gbIsService;

void MainFrame::InitDialog(WPARAM wParam, LPARAM lParam)
{

	aAppl = GlobalAddAtom(pMgr->MgrName);
	aSystemTopic = GlobalAddAtom(TOPIC_SYSTEM);

	/////////////////////////////////////////////////////
	AboutSheet *pAbout;
	Sheet about;
	pAbout = new AboutSheet(hDlg);
	pAbout->pMgr = pMgr;
	pAbout->Create(pMgr->hInstance);
	about.hDlg = pAbout->hDlg;
	about.trId = -1;
	strcpy(about.title, "About");
	ShowSheet(AddSheet(&about));
	/////////////////////////////////////////////////////
	TrayIconRegister(IDI_TRAY_NORMAL);
	char trayTipString[MAX_PATH];
	sprintf(trayTipString, "%s@%s", pMgr->MgrName, pMgr->LocalPortName);
	SetTrayTip(trayTipString);
	/////////////////////////////////////////////////////
	SetWindowText(GetDlgItem(IDC_LOCAL_PORT), pMgr->LocalPortName);
	EnableApplyButton(FALSE);
	/////////////////////////////////////////////////////
	HMENU hMenu;
	hMenu = GetSystemMenu(hDlg, FALSE);
	EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND | MF_GRAYED);
	/////////////////////////////////////////////////////
	HICON hIconBig, hIconSmall;
	hIconBig = LoadIcon(pMgr->hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	hIconSmall = LoadIcon(pMgr->hInstance, MAKEINTRESOURCE(IDI_TRAY_NORMAL));
	SendMessage(hDlg, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIconBig);
	SendMessage(hDlg, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIconSmall);
	/////////////////////////////////////////////////////
}

void MainFrame::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR *pNmhdr = (LPNMHDR) lParam; 
	
	if (wParam == IDC_SHEETTAB) {
		if (pNmhdr->code == TCN_SELCHANGE) {
			int idTabCtl = TabCtrl_GetCurSel(GetDlgItem(IDC_SHEETTAB));
			ShowSheet(idTabCtl);
		}
	}

}

void MainFrame::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_RBUTTONDOWN || lParam == WM_LBUTTONDOWN) {
		ShowPopupMenu();
	}
}

void MainFrame::OnClose(WPARAM wParam, LPARAM lParam)
{
	GetUserStation(FALSE);
	if (pMgr->OSVer.dwMajorVersion < 4) {
	} else {
		CANCEL_Clicked(0, 0);
//		Show(FALSE);
	}
}

void MainFrame::OnHelp(WPARAM wParam, LPARAM lParam)
{
	LPHELPINFO lphi = (LPHELPINFO) lParam;

	if (lphi->iContextType == HELPINFO_MENUITEM) {
	} else {
		int iCurSheet = TabCtrl_GetCurSel(GetDlgItem(IDC_SHEETTAB));
		int idTr = Sheets[iCurSheet].trId;
		PCommTransport *pTr = pMgr->GetTransportById(idTr);
		if (pTr != NULL) {
 			pTr->Control(CM_SHOWHELP, NULL);
		} else {
		}
	}
}

///////////////////////////////////////////////////////////////
#define DEFER_TRAY_ICON		1
#define DEFER_TRAY_TIP		2

void MainFrame::OnDefer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case DEFER_TRAY_ICON:
		SetTrayIcon((int) lParam);
		break;
	case DEFER_TRAY_TIP:
		SetTrayTip((char *) lParam);
		break;
	}

}

void MainFrame::SetTrayIconDefer(int iid)
{
	PostMessage(hDlg, MYWM_DEFERWORK, (WPARAM) DEFER_TRAY_ICON, (LPARAM) iid);
}

void MainFrame::SetTrayTipDefer(char *pTip)
{
	PostMessage(hDlg, MYWM_DEFERWORK, (WPARAM) DEFER_TRAY_TIP, (LPARAM) pTip);
}

///////////////////////////////////////////////////////////////

void MainFrame::OK_Clicked(WPARAM wParam, LPARAM lParam)
{
	APPLY_Clicked(wParam, lParam);
	//
	Show(FALSE);
}

void MainFrame::CANCEL_Clicked(WPARAM wParam, LPARAM lParam)
{
	int i;
	int idTrans;

	for (i=0;i<nSheets;i++) {
		idTrans = Sheets[i].trId;
		if (idTrans >= 0)
			(pMgr->GetTransportById(idTrans))->Control(CM_CANCELCHANGE, (void *) NULL);
	}
	EnableApplyButton(FALSE);
	Show(FALSE);
}

void MainFrame::APPLY_Clicked(WPARAM wParam, LPARAM lParam)
{
	int i;
	int idTrans;

	for (i=0;i<nSheets;i++) {
		idTrans = Sheets[i].trId;
		if (idTrans >= 0)
			(pMgr->GetTransportById(idTrans))->Control(CM_APPLYCHANGE, (void *) NULL);
	}
	EnableApplyButton(FALSE);
}

void MainFrame::LOCAL_PORT_Clicked(WPARAM wParam, LPARAM lParam)
{
	pMgr->DoLocalPortChange();
}

void MainFrame::SHOW_Menu(WPARAM wParam, LPARAM lParam)
{
	Show(TRUE);
}

void MainFrame::HELP_Menu(WPARAM wParam, LPARAM lParam)
{
	pMgr->OpenHelpPage(0);
}

// make it global for keep data remaining end of process.
ShutDialog shutDlg;

void MainFrame::EXIT_Menu(WPARAM wParam, LPARAM lParam)
{
	shutDlg.Create(pMgr->hInstance);
	EndDialog(hDlg, IDOK);
	TrayIconRemove();
	pMgr->ExitManager();
	EndDialog(shutDlg.hDlg, IDOK);
	PostQuitMessage(0);
//	EndDialog(shutDlg.hDlg, IDOK);
}

void MainFrame::Mode_Changed(WPARAM wParam, LPARAM lParam)
{
	int i;
	HWND hCB = (HWND) lParam;
	int idList = SendMessage(hCB, CB_GETCURSEL, 0, 0);
	if (ChangeTransport(idList))
		return;
	// restore to old selection
	int idTr = pMgr->idCurrentTransport;
	for (i=0;i<nTransports;i++) {
		if (trIds[i] == idTr) {
			ComboBoxSetCurSel(hCB, i);
			break;
		}
	}
}

void MainFrame::OnQueryEndSession(WPARAM wParam, LPARAM lParam)
{
	if (lParam)
		pMgr->Control(MCC_SHUTDOWN, NULL);
}

void MainFrame::OnUserChanged(WPARAM wParam, LPARAM lParam)
{

}

void MainFrame::OnDDEInitiate(WPARAM wParam, LPARAM lParam)
{
	int i;
	ATOM curTopic;

	if (bDDEConnected) {
		return ;
	}
	if (LOWORD(lParam) != 0 && HIWORD(lParam) != 0 &&
		(ATOM)LOWORD(lParam) == aAppl) {
		if ((ATOM)HIWORD(lParam) == aSystemTopic) {
			bDDEConnected = TRUE;
			XsDDEConnected = NULL;
			curTopic = aSystemTopic;
		} else {

			for (i=0;i<nTransports;i++) {
				if ((ATOM) HIWORD(lParam) == trTopics[i]) {
					curTopic = trTopics[i];
					XsDDEConnected = pMgr->GetTransportById(trIds[i]);
					if (XsDDEConnected != NULL)
						bDDEConnected = TRUE;
				}
			}
		}
	}
	if (bDDEConnected) {
		char atomName[MAX_PATH];
		// make duplicates of the incoming atoms (really adding a reference)
		ATOM aApplTmp = GlobalAddAtom(SERVICENAME);
		GlobalGetAtomName(curTopic, atomName, MAX_PATH);
		ATOM aTopicTmp = GlobalAddAtom(atomName);

		// send the WM_DDE_ACK (caller will delete duplicate atoms)
		::SendMessage((HWND)wParam, WM_DDE_ACK, 
			(WPARAM)hDlg,
			MAKELPARAM(aApplTmp, aTopicTmp));
	} else {
		XsDDEConnected = NULL;
	}

}

void MainFrame::OnDDEExecute(WPARAM wParam, LPARAM lParam)
{
	if (!bDDEConnected) {
		return ;
	}
	// unpack the DDE message
	XS_DDE ddePacket;
	TCHAR *pszCommand;
	UINT unused;
	HGLOBAL hData;
	LPCTSTR lpsz;

	UnpackDDElParam(WM_DDE_EXECUTE, lParam, &unused, (UINT*)&hData);
	// get the command string
	lpsz = (LPCTSTR)GlobalLock(hData);
	//strncpy(szCommand, lpsz, MAX_PATH * 2);
	pszCommand = strdup(lpsz);
	GlobalUnlock(hData);
	// acknowledge now - before attempting to execute
	::PostMessage((HWND)wParam, WM_DDE_ACK, 
		(WPARAM)hDlg,
		ReuseDDElParam(lParam, WM_DDE_EXECUTE, WM_DDE_ACK,
		(UINT)0x8000, (UINT)hData));

	ddePacket.command = XS_DDE_EXECUTE;
	ddePacket.ddeData.pExecString = pszCommand;

	if (XsDDEConnected == NULL) {
		// connected to toplevel of application, do my work
		//
		ProcessDDE(&ddePacket);
		//
		//
	} else {
		XsDDEConnected->Control(CM_DDE, (void *)&ddePacket);
	}
	free(pszCommand);
}

void MainFrame::OnDDETerminate(WPARAM wParam, LPARAM lParam)
{
	if (!bDDEConnected) {
		return ;
	}
	bDDEConnected = FALSE;
	if (XsDDEConnected == NULL) {
	} else {
	}
	::PostMessage((HWND)wParam, WM_DDE_TERMINATE, 
		(WPARAM)hDlg, lParam);
}

void MainFrame::OnDDERequest(WPARAM wParam, LPARAM lParam)
{
	XS_DDE ddePacket;
	char AtomName[MAX_PATH];
	HGLOBAL hData;
	DDEDATA *pData;

	if (!bDDEConnected) {
		return ;
	}
	ddePacket.command = XS_DDE_REQUEST;
	GlobalGetAtomName(HIWORD(lParam), AtomName, MAX_PATH);
	ddePacket.ddeData.requestData.pReqItem = AtomName;
	ddePacket.ddeData.requestData.format = LOWORD(lParam);
	ddePacket.ddeData.requestData.pData = NULL;
	ddePacket.ddeData.requestData.sizeData = 0;
	if (XsDDEConnected == NULL) {
		// this is for me, do my work
		ProcessDDE(&ddePacket);
	} else {
		XsDDEConnected->Control(CM_DDE, (void *)&ddePacket);
	}
	if (ddePacket.ddeData.requestData.pData) {
		hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 
				(LONG) sizeof(DDEDATA) + ddePacket.ddeData.requestData.sizeData + 2);
		if (!hData) {
		}
		pData = (DDEDATA *) GlobalLock(hData);
		if (!pData) {
		}
		pData->cfFormat = ddePacket.ddeData.requestData.format;
		memcpy((void *) pData->Value, 
				ddePacket.ddeData.requestData.pData, 
				ddePacket.ddeData.requestData.sizeData);

		pData->fResponse = 1;

		GlobalUnlock(hData);

		::PostMessage((HWND)wParam, WM_DDE_DATA, 
			(WPARAM)hDlg,
			ReuseDDElParam(lParam, WM_DDE_REQUEST, WM_DDE_DATA,
			(UINT) hData,
			(UINT) HIWORD(lParam)));
	} else {
		// reject
		DDEACK *pAckVal;
		UINT ackVal;
		pAckVal = (DDEACK *) &ackVal;
		pAckVal->fAck = 0;
		pAckVal->fBusy = 0;
		pAckVal->bAppReturnCode = 0;
		::PostMessage((HWND)wParam, WM_DDE_ACK, 
			(WPARAM)hDlg,
			ReuseDDElParam(lParam, WM_DDE_REQUEST, WM_DDE_ACK,
			(UINT)ackVal, (UINT) HIWORD(lParam)));
	}
}

/////////////////////////////////////////////////////////

int MainFrame::AddSheet(Sheet *pSheet)
{
	if (nSheets > MAX_TRANSPORTS)
		return -1;
	ShowWindow(pSheet->hDlg, SW_HIDE);
	Sheets[nSheets].hDlg = pSheet->hDlg;
	//strncpy(Sheets[nSheets].title, pSheet->title, 256);
	strcpy(Sheets[nSheets].title, pSheet->title);
	Sheets[nSheets].trId = pSheet->trId;
	// insert tab item
	HWND hTabCtrl = GetDlgItem(IDC_SHEETTAB);
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	TabCtrlItem.pszText = pSheet->title;
	TabCtrl_InsertItem(hTabCtrl, nSheets, &TabCtrlItem);
	// reparent sheet
	SetParent(pSheet->hDlg, hTabCtrl);
	// resize tab control
	RECT rc1, rcNew, rcOld;
	POINT pts, pte;
	GetWindowRect(hTabCtrl, &rc1);
	TabCtrl_AdjustRect(hTabCtrl, FALSE, &rc1);
	pts.x = rc1.left; pts.y = rc1.top;
	ScreenToClient(hTabCtrl, &pts);
	SetWindowPos(pSheet->hDlg, HWND_TOP, pts.x, pts.y, 0, 0, SWP_NOSIZE);
	GetWindowRect(pSheet->hDlg, &rc1);
	TabCtrl_AdjustRect(hTabCtrl, TRUE, &rc1);
	pts.x = rc1.left; pts.y = rc1.top;
	pte.x = rc1.right; pte.y = rc1.bottom;
	ScreenToClient(hDlg, &pts);
	ScreenToClient(hDlg, &pte);
	GetWindowRect(hTabCtrl, &rcOld);
	pts.x = pte.x - pts.x; 
	if (pts.x < rcOld.right - rcOld.left) 
		pts.x = rcOld.right - rcOld.left;
	pts.y = pte.y - pts.y;
	if (pts.y < rcOld.bottom - rcOld.top)
		pts.y = rcOld.bottom - rcOld.top;
	SetWindowPos(hTabCtrl, HWND_TOP,
				0, 0,
				pts.x, pts.y, SWP_NOMOVE);
	// resize frame dialog
	GetWindowRect(hTabCtrl, &rcNew);
	pte.x = rcNew.right - rcOld.right;
	pte.y = rcNew.bottom - rcOld.bottom;
	if (pte.x < 0) pte.x = 0;
	if (pte.y < 0) pte.y = 0;
		// move buttons
	GetWindowRect(GetDlgItem(IDC_OK), &rcOld);
	pts.x = rcOld.left + pte.x; pts.y = rcOld.top + pte.y;
	ScreenToClient(hDlg, &pts);
	SetWindowPos(GetDlgItem(IDC_OK), HWND_TOP,
			pts.x, pts.y, 0, 0, SWP_NOSIZE);
	GetWindowRect(GetDlgItem(IDC_CANCEL), &rcOld);
	pts.x = rcOld.left + pte.x; pts.y = rcOld.top + pte.y;
	ScreenToClient(hDlg, &pts);
	SetWindowPos(GetDlgItem(IDC_CANCEL), HWND_TOP,
			pts.x, pts.y, 0, 0, SWP_NOSIZE);
	GetWindowRect(GetDlgItem(IDC_APPLY), &rcOld);
	pts.x = rcOld.left + pte.x; pts.y = rcOld.top + pte.y;
	ScreenToClient(hDlg, &pts);
	SetWindowPos(GetDlgItem(IDC_APPLY), HWND_TOP,
			pts.x, pts.y, 0, 0, SWP_NOSIZE);
		// resize sheet
	GetWindowRect(hDlg, &rcOld);
	SetWindowPos(hDlg, HWND_TOP, 0, 0,
						rcOld.right - rcOld.left + pte.x,
						rcOld.bottom - rcOld.top + pte.y,
						SWP_NOMOVE);


	return nSheets++;
}

int MainFrame::ShowSheet(int idSheet)
{
	if (idSheet >= nSheets)
		return FALSE;
	int i;
	for (i=0;i<nSheets;i++) {
		if (idSheet == i)
			ShowWindow(Sheets[i].hDlg, SW_SHOW);
		else
			ShowWindow(Sheets[i].hDlg, SW_HIDE);
	}
	return TRUE;
}

BOOL MainFrame::TrayMessage(DWORD dwMessage, UINT uFlags, UINT uID, HICON hIcon, PSTR pszTip)
{
    BOOL res;

	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= hDlg;
	tnd.uID			= uID;

	tnd.uFlags		= uFlags; //NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage	= MYWM_NOTIFYICON;
	tnd.hIcon		= hIcon;
	if (pszTip)
	{
		lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip));
	}
	else
	{
		tnd.szTip[0] = '\0';
	}

	res = Shell_NotifyIcon(dwMessage, &tnd);

	if (hIcon)
	    DestroyIcon(hIcon);

	return res;
}

BOOL MainFrame::SetTrayTip(char *pStr)
{
	return TrayMessage(NIM_MODIFY, 
				NIF_MESSAGE|NIF_TIP, 
				IDC_DUMM, NULL, pStr);
}

BOOL MainFrame::SetTrayIcon(int iid)
{
	HICON hIcon;

	if (curIcon == iid)
		return TRUE;
	curIcon = iid;
	hIcon = LoadIcon(pMgr->hInstance, MAKEINTRESOURCE(iid));
	return TrayMessage(NIM_MODIFY, 
				NIF_MESSAGE|NIF_ICON, 
				IDC_DUMM, hIcon, NULL);
}

BOOL MainFrame::TrayIconRegister(int iid)
{
	HICON hIcon;
	hIcon = LoadIcon(pMgr->hInstance, MAKEINTRESOURCE(iid));
	return TrayMessage(NIM_ADD, 
				NIF_MESSAGE|NIF_ICON, 
				IDC_DUMM, hIcon, NULL);
}

BOOL MainFrame::TrayIconRemove()
{
	return TrayMessage(NIM_DELETE,
				NIF_MESSAGE|NIF_ICON|NIF_TIP,
				IDC_DUMM, NULL, NULL);
}

void MainFrame::Show(BOOL bShow)
{
	ShowWindow(hDlg, bShow);
}

void MainFrame::EnableApplyButton(BOOL bEnable)
{
	EnableWindow(GetDlgItem(IDC_APPLY), bEnable);
}

void MainFrame::ShowPopupMenu()
{
	HMENU myMenu, myPopup;
	char tmpStr[256];
	int i;
	POINT pt;

	myMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
	myPopup = GetSubMenu(myMenu, 0);
	if (myPopup == NULL) {
		int err = GetLastError();
	}
	if (!GetCursorPos(&pt)) {
		int err = GetLastError();
	}

	// setup static items
	sprintf(tmpStr, "%s(%s)", APPNAME, pMgr->LocalPortName);
	ModifyMenu(myPopup, 0, MF_BYPOSITION | MF_STRING,
			ID_MENUITEM32775 , tmpStr);
	if (pMgr->bOpened) {
		EnableMenuItem(myPopup, ID_EXIT, MF_BYCOMMAND | MF_GRAYED);
	}

	// setup dynamic items
		// setup transport menu items
	for (i=0;i<nTransports;i++) {
		InsertMenu(myPopup, MODEMENUPOS + i, 
			MF_BYPOSITION | MF_STRING,
			ID_MODEMENU_START + i,
			pTrNames[i]);
		if (trIds[i] == pMgr->idCurrentTransport)
			CheckMenuItem(myPopup,
				MODEMENUPOS + i,
				MF_BYPOSITION | MF_CHECKED);
		if (pMgr->bOpened)
			EnableMenuItem(myPopup, MODEMENUPOS + i, MF_BYPOSITION | MF_GRAYED);
	}
		// setup Ex menu items
	if (pMenuArray != NULL) {
		for (i=0;i<pMenuArray->nItems;i++) {
			InsertMenu(myPopup, EXMENUPOS + i, 
				MF_BYPOSITION | MF_STRING,
				ID_EXMENU_START + i,
				pMenuArray->items[i].DisplayName);
			if (pMenuArray->items[i].state & EX_MS_CHECKED)
				CheckMenuItem(myPopup,
					EXMENUPOS + i,
					MF_BYPOSITION | MF_CHECKED);
			if (pMgr->bOpened || pMenuArray->items[i].state & EX_MS_GRAYED)
				EnableMenuItem(myPopup, EXMENUPOS + i, MF_BYPOSITION | MF_GRAYED);
		}
		if (pMenuArray->nItems > 0) {
			InsertMenu(myPopup, EXMENUPOS + pMenuArray->nItems, 
				MF_BYPOSITION | MF_SEPARATOR,
				0,
				0);
		}
	}
	//

	// do menu action
	SetActiveWindow(hDlg);
	TrackPopupMenuEx(myPopup,
		TPM_RIGHTALIGN | TPM_LEFTBUTTON,
		pt.x, pt.y, hDlg, NULL);
	DestroyMenu(myPopup);

}

int MainFrame::ChangeTransport(int index)
{
	ExMenuArray *pMenuArrayOld = pMenuArray;
	pMenuArray = NULL;
	if (!pMgr->TransportSelect(trIds[index])) {
		pMenuArray = pMenuArrayOld;
		return FALSE;
	}

	return TRUE;
}

int MainFrame::AddXPMode(int trId, char *pName)
{
	if (trId > MAX_TRANSPORTS)
		return FALSE;
	trIds[nTransports] = trId;
	pTrNames[nTransports] = strdup(pName);
	// for DDE
	if (trTopics[nTransports])
		DeleteAtom(trTopics[nTransports]);
	trTopics[nTransports] = GlobalAddAtom(pName);
	//
	HWND hCB = GetDlgItem(IDC_SELMODE);
	ComboBoxInsertString(hCB, nTransports, pName);
	nTransports++;
	return TRUE;
}

int MainFrame::DeleteXPMode(int trId)
{
	int i, j;
	for (i=0;i<nTransports;i++) {
		if (trIds[i] == trId) {
			// delete atom for DDE
			if (trTopics[i])
				DeleteAtom(trTopics[i]);
			//
			if (pTrNames[i] != NULL) {
				free(pTrNames[i]);
				pTrNames[i] = NULL;
			}
			for (j=i+1;j<MAX_TRANSPORTS;j++) {
				trIds[j-1] = trIds[j];
				pTrNames[j-1] = pTrNames[j];
				trTopics[j-1] = trTopics[j];
			}
			nTransports--;
			HWND hCB = GetDlgItem(IDC_SELMODE);
			ComboBoxResetContent(hCB);
			for (j=0;j<nTransports;j++) {
				ComboBoxInsertString(hCB, j, pTrNames[j]);
			}
			return TRUE;
		}
	}
	return FALSE;
}

void MainFrame::SetInitialXPMode(int trId)
{
	int i;
	for (i=0;i<nTransports;i++) {
		if (trIds[i] == trId) {
			ComboBoxSetCurSel(GetDlgItem(IDC_SELMODE), i);
			break;
		}
	}
}

int MainFrame::Control(int Command, void *pCd)
{

	TrayData *pTrayCommand = (TrayData *) pCd;
	switch (Command) {
	case UIC_SHEET_ADD:
		AddSheet((Sheet *) pCd);
		break;
	case UIC_SHEET_DEL:
		break;
	case UIC_MENUARRAY_SET:
		pMenuArray = (ExMenuArray *) pCd;
		break;
	case UIC_MENUARRAY_DELETE:
		pMenuArray = NULL;
		break;
	case UIC_TRAY_SETTIP:
		SetTrayTipDefer(pTrayCommand->pTipString);
		break;
	case UIC_TRAY_CHANGEICON:
		switch (pTrayCommand->iconKind) {
		case TI_NORMAL:
			SetTrayIconDefer(IDI_TRAY_NORMAL);
			return TRUE;
		case TI_CONNECT:
			SetTrayIconDefer(IDI_TRAY_ON);
			return TRUE;
		case TI_BADSTATE:
			SetTrayIconDefer(IDI_TRAY_STOP);
			return TRUE;
		default:
			return FALSE;
		}
		break;
	case UIC_SHEET_CHANGED:
		EnableApplyButton(TRUE);
		break;
	}

	return TRUE;
}

void MainFrame::ProcessDDE(XS_DDE *pPack)
{
	static char resultBuf[1024];
	int i;
	if (pPack->command == XS_DDE_EXECUTE) {
		char *pCommandString;
		char *pCommand, *pArg1;
		resultBuf[0] = (char) 0;
		pCommandString = strdup(pPack->ddeData.pExecString);
		pCommand = strtok(pCommandString, ":");
		pArg1 = strtok(NULL, ", ");
		if (strcmp(pCommand, SETFUNCTIONMODE) == 0) {
			if (pArg1 == NULL) {
				for (i=0;i<nTransports;i++) {
					if (trIds[i] == pMgr->idCurrentTransport)
						strncpy(resultBuf, pTrNames[i], 1023);
				}
			} else {
				int vInt = atoi(pArg1);
				int ret;
				if (vInt < nTransports) {
					ret = ChangeTransport(vInt);
					if (ret)
						strncpy(resultBuf, pTrNames[vInt], 1023);
					else
						strcpy(resultBuf, "Could not change transport");
				} else {
					strcpy(resultBuf, "Bad index");
				}
			}
		} else
		if (strcmp(pCommand, RELOADSETTING) == 0) {
			for (i=0;i<nTransports;i++) {
				PCommTransport *pTransport = 
					pMgr->GetTransportById(trIds[i]);
				if (pTransport) 
					pTransport->Control(CM_RELOAD_SETTING, NULL);
			}
		} else {
			strcpy(resultBuf, "Bad command");
		}
		free(pCommandString);
		return;
	}
	if (pPack->command == XS_DDE_REQUEST) {
		if (strcmp(pPack->ddeData.requestData.pReqItem, RESULT_ITEM) == 0 &&
			pPack->ddeData.requestData.format == CF_TEXT) {
			pPack->ddeData.requestData.pData = resultBuf;
			pPack->ddeData.requestData.sizeData = strlen(resultBuf) + 1;
		}
	}

}
