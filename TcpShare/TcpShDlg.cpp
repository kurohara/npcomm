/***********************************************************************
    TcpShDlg.cpp
    The Dialog base user interface implementation part of TcpShare.xpm.

	Copyright (C) 1999-2003 by Hiroyoshi Kurohara
		All Rights Reserved

  	This file is part of TcpShare XP module(transport DLL for NPCOMM).

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
#include <COMMCTRL.h>
#include <stdio.h>
#include "PCommMgr.h"
#include "TcpShare.h"
#include "TcpShDlg.h"

extern HINSTANCE gMyHandle;
extern int XpmMessageBox(unsigned int resId, unsigned int mask);

#define SetModified(b) (b ? pMyTransport->MgrUIControl(UIC_SHEET_CHANGED, NULL) : TRUE)

#define MYWM_SOCKET (WM_USER+100)
#define MYTIMER_ID			100
#define MYTIMER_INTERVAL	1500
///////////////////////////////////////////////////////////////////////
static MessageMap myMap[] = {
	MESSAGE_HANDLER(TcpShareDlg, MYWM_SOCKET, OnSockNotify),
	MESSAGE_HANDLER(TcpShareDlg, WM_INITDIALOG, InitDialog),
	MESSAGE_HANDLER(TcpShareDlg, WM_NOTIFY, OnNotify),
	MESSAGE_HANDLER(TcpShareDlg, WM_TIMER, OnTimer),
	// push buttons
	COMMAND_HANDLER(TcpShareDlg, BN_CLICKED, IDC_GETSERVER, GETSERVER_Clicked),
	COMMAND_HANDLER(TcpShareDlg, BN_CLICKED, IDC_ADDTOLIST, ADDTOLIST_Clicked),
	COMMAND_HANDLER(TcpShareDlg, BN_CLICKED, IDC_REMOVEFROMLIST, REMOVEFROMLIST_Clicked),
	COMMAND_HANDLER(TcpShareDlg, BN_CLICKED, IDC_GETSERVSTATUS, GETSERVERSTATUS_Clicked),
	COMMAND_HANDLER(TcpShareDlg, BN_CLICKED, IDC_PINGER, PINGER_Clicked),
	// check boxes
	COMMAND_HANDLER(TcpShareDlg, BN_CLICKED, IDC_AUTOCHECK, AUTOCHECK_Clicked),
	COMMAND_HANDLER(TcpShareDlg, BN_CLICKED, IDC_DOTOUCH, DOTOUCH_Clicked),
	//
	COMMAND_HANDLER(TcpShareDlg, CBN_SELCHANGE, IDC_COMBO_REMOTEM, REMOTEM_SelChange),
	COMMAND_HANDLER(TcpShareDlg, CBN_EDITUPDATE, IDC_COMBO_REMOTEM, REMOTEM_EditChange),
	COMMAND_HANDLER(TcpShareDlg, CBN_SELCHANGE, IDC_COMBO_REMOTEP, REMOTEP_SelChange),
	//
	{ 0, 0, 0, NULL }
} ;
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void ListView_Clear(HWND hLV)
{
	int i, n;
	n = ListView_GetItemCount(hLV);
	for (i=0;i<n;i++) {
		ListView_DeleteItem(hLV, 0);
	}
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
TcpShareDlg::TcpShareDlg(HWND parent)
: aDialog(TcpShareDlg::IDD, parent)
{
	////////////////////////////////////////////
	pMesMap = (void *) myMap;
	////////////////////////////////////////////
	int i;

	params.RemoteHostName[0] = (char) 0;
	for (i=0;i<NUM_HOSTNAMES;i++) {
		params.RemoteHostNames[i][0] = (char) 0;
	}
	////////////////////////////////////////////

	// allocate memory for ex menu
	pExMenu = (ExMenuArray *) malloc(
					sizeof (ExMenuArray) 
					+ MAX_SETTINGS * sizeof (ExMenuItem));
	pExMenu->nItems = 0;
	for (i=0;i<MAX_SETTINGS;i++) {
		pExMenu->items[i].pData = NULL;
		pExMenu->items[i].state = EX_MS_NORMAL;
	}
	////////////////////
	tcnt_GETSERVER = 0;

	uSock.pCallback = this;
}

TcpShareDlg::~TcpShareDlg()
{

	free(pExMenu);
}

void TcpShareDlg::InitDialog(WPARAM wParam, LPARAM lParam)
{

	// set signature string to label
	SetWindowText(GetDlgItem(IDC_DIALOGLABEL), XPM_SIGNATURE);
	//

	pMyTransport->GetParameters(&params);
	uSock.servicePortNumber = params.IPServicePortNum;

	SetupListview();
	int i;
	char **pPortNameList = pMyTransport->getPortNameList();
	for (i=0;i<pMyTransport->getNumPortNames();i++) {
		ComboBoxAddString(GetDlgItem(IDC_COMBO_REMOTEP), 
			pPortNameList[i]);
	}

	SetDlgItemData();
	//////////////
	// for ex menu
	// set up data for Ex menu
	pExMenu->trId = pMyTransport->getTransportId();
	pExMenu->nItems = params.nSettingItems;
	PSSettingItem *pItem = params.settingItems;
	ExMenuItem *pMenuItem = pExMenu->items;
	for (i=0;i<pExMenu->nItems;i++) {
		if (pItem->pAlias[0] != 0)
			strncpy(pMenuItem->DisplayName, pItem->pAlias, MAX_PATH);
		else
			_snprintf(pMenuItem->DisplayName, MAX_PATH, "%s - %s",
				pItem->pHostName,
				pItem->pPortName);
		if (strcmp(pItem->pHostName, params.RemoteHostName) == 0 &&
			strcmp(pItem->pPortName, params.RemotePortName) == 0) {
			pMenuItem->state = EX_MS_CHECKED;
		}
		//
		pItem++;
		pMenuItem++;
	}
	//

	SetTimer(hDlg, MYTIMER_ID, MYTIMER_INTERVAL, NULL);
	uSock.SetWindowNotify(hDlg, MYWM_SOCKET);
}

int TcpShareDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return aDialog::OnCommand(wParam, lParam);
}

void TcpShareDlg::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LV_DISPINFO *pdi = (LV_DISPINFO *) lParam;

	if (pdi->hdr.code == LVN_ENDLABELEDIT) {
		if (pdi->item.pszText != NULL && pdi->item.iItem != -1) {
			if (strchr(pdi->item.pszText, (int) ',') != NULL)
				return;
			if (strchr(pdi->item.pszText, (int) ':') != NULL)
				return;
		} else
			return;

		ListView_SetItem(GetDlgItem(IDC_SETTINGLIST), &pdi->item);
		SetModified(TRUE);
	} else
	if (pdi->hdr.code == NM_DBLCLK) {
		OnDblClickControl();
	}
}

void TcpShareDlg::OnDblClickControl()
{
	LV_HITTESTINFO  htInfo;
	HWND hWndLV = GetDlgItem(IDC_SETTINGLIST);
	int rpIndex;
	char tmpPortName[MAX_PATH];
	char tmpHostName[MAX_PATH];
	//
	GetCursorPos(&htInfo.pt);
	ScreenToClient(hWndLV, &htInfo.pt);
	htInfo.flags = LVHT_ONITEMICON | LVHT_ONITEMLABEL;

	if (ListView_HitTest(hWndLV, &htInfo) < 0)
		return;

	// set  current setting 
	ListView_GetItemText(hWndLV, htInfo.iItem, 2, tmpPortName, MAX_PATH);
	ListView_GetItemText(hWndLV, htInfo.iItem, 1, tmpHostName, MAX_PATH);
	rpIndex = atoi(&tmpPortName[3]);
	if (rpIndex < 1)
		return;
	rpIndex--;
	ComboBoxSetCurSel(GetDlgItem(IDC_COMBO_REMOTEP), rpIndex);
	SetWindowText(GetDlgItem(IDC_COMBO_REMOTEM), tmpHostName);

}

void TcpShareDlg::OnTimer(WPARAM wParam, LPARAM lParam)
{
	TrayData tData;
	//
	//
	if (params.bAutoCheck && !pMyTransport->isConnected() && bSelected) {
		if (bConnectable) {
			tData.iconKind = TI_NORMAL;
			pMyTransport->MgrUIControl(UIC_TRAY_CHANGEICON, &tData);
		} else {
			tData.iconKind = TI_BADSTATE;
			pMyTransport->MgrUIControl(UIC_TRAY_CHANGEICON, &tData);
		}
		bConnectable = FALSE;
		uSock.CheckServerPresense(params.RemoteHostName, params.RemotePortName);
	}
	// check if timeout of GETSERVER expired
	if (tcnt_GETSERVER) {
		tcnt_GETSERVER--;
		if (tcnt_GETSERVER == 0) {
			XpmMessageBox(IDS_NO_PORTSERV, MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
		}
	}
}

void TcpShareDlg::OnSockNotify(WPARAM wParam, LPARAM lParam)
{
	int cEvent = WSAGETSELECTEVENT(lParam);
	if (cEvent != FD_READ)
		return;

	// this may call back my UDPCallback methods
	uSock.Recv();
}

///////////////////////////////////////////////////////////////////////
// Push buttons
void TcpShareDlg::GETSERVER_Clicked(WPARAM wParam, LPARAM lParam)
{

	uSock.SearchServer();

	tcnt_GETSERVER = 5;
}

void TcpShareDlg::ADDTOLIST_Clicked(WPARAM wParam, LPARAM lParam)
{
	PSSettingItem sItem;
	char alias[1], hostName[MAX_PATH], portName[MAX_PATH];
	sItem.pAlias = alias; sItem.pHostName = hostName; sItem.pPortName = portName;
	alias[0] = (char) 0;
	int lenHostName =
		GetWindowText(GetDlgItem(IDC_COMBO_REMOTEM), hostName, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_COMBO_REMOTEP), portName, MAX_PATH);
	if (lenHostName == 0)
		return;
	AddLVEntry(&sItem);
	SetModified(TRUE);
}

void TcpShareDlg::REMOVEFROMLIST_Clicked(WPARAM wParam, LPARAM lParam)
{
	int i, nItems;
	HWND hWndLV = GetDlgItem(IDC_SETTINGLIST);
	nItems = ListView_GetItemCount(hWndLV);

	// delete all selected items
	for (i=0;i<nItems;i++) {
		if (ListView_GetItemState(hWndLV, i, LVIS_SELECTED) != 0) {
			ListView_DeleteItem(hWndLV, i);
			SetModified(TRUE);
		}
	}

}

void TcpShareDlg::GETSERVERSTATUS_Clicked(WPARAM wParam, LPARAM lParam)
{
	PrintToResultEdit("Server\tClient\tPort\r\n----------------------------\r\n");
	uSock.StatusRequest(NULL); // NULL means broadcast
}

void TcpShareDlg::PINGER_Clicked(WPARAM wParam, LPARAM lParam)
{
	TcpShareParams oldParams;
	int mStat;

	if (pMyTransport->isConnected()) {
		PrintToResultEdit("Already connected!");
		return;
	}

	pMyTransport->GetParameters(&oldParams);
	//
	GetWindowText(GetDlgItem(IDC_COMBO_REMOTEM), params.RemoteHostName, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_COMBO_REMOTEP), params.RemotePortName, 32);
	int bTouch = ButtonGetCheck(GetDlgItem(IDC_DOTOUCH));
	//
	pMyTransport->PutParameters(&params);
	PrintToResultEdit("Checking....");
	if (pMyTransport->ConnectionTest(bTouch, &mStat)) {
		char tmpBuf[MAX_PATH];
		if (bTouch) 
			_snprintf(tmpBuf, MAX_PATH, "OK : %d %d %d %d",
				(mStat & 0x10) >> 4,
				(mStat & 0x20) >> 5,
				(mStat & 0x40) >> 6,
				(mStat & 0x80) >> 7);
		else
			strcpy(tmpBuf, "OK!");
		PrintToResultEdit(tmpBuf);
		AddHostEntryToList(params.RemoteHostName);
	} else {
		if (mStat) {
			PrintToResultEdit("Port occupied!");
		} else {
			PrintToResultEdit("No responce!");
		}
	}
	//
	//
	pMyTransport->PutParameters(&oldParams);
}

///////////////////////////////////////////////////////////////////////
// Check boxes
void TcpShareDlg::AUTOCHECK_Clicked(WPARAM wParam, LPARAM lParam)
{
	SetModified(TRUE);
}

void TcpShareDlg::DOTOUCH_Clicked(WPARAM wParam, LPARAM lParam)
{
}

///////////////////////////////////////////////////////////////////////
// Other controls
void TcpShareDlg::REMOTEM_SelChange(WPARAM wParam, LPARAM lParam)
{
	SetModified(TRUE);
}

void TcpShareDlg::REMOTEM_EditChange(WPARAM wParam, LPARAM lParam)
{
	SetModified(TRUE);
}

void TcpShareDlg::REMOTEP_SelChange(WPARAM wParam, LPARAM lParam)
{
	SetModified(TRUE);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// Utilities
void TcpShareDlg::SetupListview()
{
	HWND hWndLView;
	HIMAGELIST hImList;
	HICON hIcon;
	ICONINFO iIcon;
	
	// set up image list
	hImList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 0, 1);
	hIcon = LoadIcon(gMyHandle, MAKEINTRESOURCE(IDI_TRAY_NORMAL));
	GetIconInfo(hIcon, &iIcon);
	ImageList_Add(hImList, iIcon.hbmColor, iIcon.hbmMask);
	DestroyIcon(hIcon);
	//
	LV_COLUMN col;
	hWndLView = GetDlgItem(IDC_SETTINGLIST);
	ListView_SetImageList(hWndLView, hImList, LVSIL_SMALL);
	col.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 70;
	col.pszText = "Alias";
	col.iSubItem = 0;
	ListView_InsertColumn(hWndLView, 0, &col);
	col.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 80;
	col.pszText = "Host Address";
	col.iSubItem = 1;
	ListView_InsertColumn(hWndLView, 1, &col);
	col.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 45;
	col.pszText = "COM port";
	col.iSubItem = 2;
	ListView_InsertColumn(hWndLView, 2, &col);

}

void TcpShareDlg::AddLVEntry(PSSettingItem *pItem)
{
	LV_ITEM aItem;
	HWND hLV = GetDlgItem(IDC_SETTINGLIST);

	// search if already exist
	int i, n;
	char tmpString[MAX_PATH];
	n = ListView_GetItemCount(hLV);
	for (i=0;i<n;i++) {
		aItem.iItem = i;
		aItem.mask = LVIF_TEXT;
		aItem.iSubItem = 1;
		aItem.pszText = tmpString;
		aItem.cchTextMax = MAX_PATH;
		ListView_GetItem(hLV, &aItem);
		if (strcmp(tmpString, pItem->pHostName) != 0)
			continue;

		aItem.iItem = i;
		aItem.mask = LVIF_TEXT;
		aItem.iSubItem = 2;
		aItem.pszText = tmpString;
		aItem.cchTextMax = MAX_PATH;
		ListView_GetItem(hLV, &aItem);
		if (strcmp(tmpString, pItem->pPortName) == 0)
			return;
	}

	// add item
	aItem.iItem = 0;
	aItem.mask = LVIF_TEXT | LVIF_IMAGE;
	aItem.iSubItem = 0;
	aItem.iImage = 0;
	aItem.pszText = pItem->pAlias;
	ListView_InsertItem(hLV, &aItem);

	aItem.iItem = 0;
	aItem.mask = LVIF_TEXT;
	aItem.iSubItem = 1;
	aItem.pszText = pItem->pHostName;

	ListView_SetItem(hLV, &aItem);

	//
	aItem.iItem = 0;
	aItem.mask = LVIF_TEXT;
	aItem.iSubItem = 2;
	aItem.pszText = pItem->pPortName;

	ListView_SetItem(hLV, &aItem);

}

void TcpShareDlg::GetLVEntry(int index, PSSettingItem *pItem)
{
	LV_ITEM aItem;
	char tmpString[MAX_PATH];
	HWND hLV = GetDlgItem(IDC_SETTINGLIST);

	aItem.iItem = index;
	aItem.mask = LVIF_TEXT;
	aItem.iSubItem = 0;
	aItem.pszText = tmpString;
	aItem.cchTextMax = MAX_PATH;
	ListView_GetItem(hLV, &aItem);
	pItem->pAlias = strdup(aItem.pszText);

	aItem.iItem = index;
	aItem.mask = LVIF_TEXT;
	aItem.iSubItem = 1;
	aItem.pszText = tmpString;
	aItem.cchTextMax = MAX_PATH;
	ListView_GetItem(hLV, &aItem);
	pItem->pHostName = strdup(aItem.pszText);

	aItem.iItem = index;
	aItem.mask = LVIF_TEXT;
	aItem.iSubItem = 2;
	aItem.pszText = tmpString;
	aItem.cchTextMax = MAX_PATH;
	ListView_GetItem(hLV, &aItem);
	pItem->pPortName = strdup(aItem.pszText);

}

void TcpShareDlg::PrintToResultEdit(char *pText,
									 int bAppend,
									 int iPos)
{
	HWND hEdit = GetDlgItem(IDC_CHECKRESULT);
	
	if (bAppend)
		SendMessage(hEdit, EM_SETSEL, (WPARAM) iPos, (LPARAM) iPos);
	else
		SendMessage(hEdit, EM_SETSEL, (WPARAM) 0, (LPARAM) -1);

	SendMessage(hEdit, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) pText);
}

void TcpShareDlg::PrintLineToResultEdit(char *pText,
										 int iLine)
{
	char tmpBuf[MAX_PATH];
	int iPos;
	HWND hEdit = GetDlgItem(IDC_CHECKRESULT);

	if (iLine == -1)
		iPos = -1;
	else
		iPos = SendMessage(hEdit, EM_LINEINDEX, (WPARAM) iLine, (LPARAM) 0);
	_snprintf(tmpBuf, MAX_PATH, "%s\r\n", pText);
	PrintToResultEdit(tmpBuf, TRUE, iPos);
}

int TcpShareDlg::SearchTextLineResultEdit(char *pText)
{
	int nLines;
	int i;
	int nLineChar;
	char tmpBuf[MAX_PATH], *pFound;

	HWND hEdit = GetDlgItem(IDC_CHECKRESULT);
	nLines = SendMessage(hEdit, EM_GETLINECOUNT, (WPARAM) 0, (LPARAM) 0);
	
	for (i=0;i<nLines;i++) {
		nLineChar = SendMessage(hEdit, EM_GETLINE, (WPARAM) i, (LPARAM) tmpBuf);
		if (nLineChar > 0) {
			tmpBuf[nLineChar] = (char) 0;
			pFound = strstr(tmpBuf, pText);
			if (pFound != NULL) {
				return i;
			}
		}
	}
	return -1;
}

void TcpShareDlg::AddHostEntryToList(char *pRemoteHostName)
{
	HWND hCB = GetDlgItem(IDC_COMBO_REMOTEM);
	if (CB_ERR == ComboBoxFindString(hCB, pRemoteHostName)) {
		ComboBoxAddString(hCB, pRemoteHostName);
		SetModified(TRUE);
	}
}

///////////////////////////////////////////////////////////////////////
// Dll local interface
void TcpShareDlg::ApplyChange()
{
	int i;
	HWND hCB = GetDlgItem(IDC_COMBO_REMOTEM);
	int n = ComboBoxGetCount(hCB);
	GetWindowText(hCB, params.RemoteHostName, MAX_PATH);
	for (i=0;i<NUM_HOSTNAMES;i++) {
		if (i < n)
			ComboBoxGetLBText(hCB, i, params.RemoteHostNames[i]);
		else
			params.RemoteHostNames[i][0] = (char) 0;
	}
	//////////////////////////////////////////
	hCB = GetDlgItem(IDC_COMBO_REMOTEP);
	n = ComboBoxGetCurSel(hCB);
	ComboBoxGetLBText(hCB, n, params.RemotePortName);
	// make setting list string from list view.
	n = ListView_GetItemCount(GetDlgItem(IDC_SETTINGLIST));
	pExMenu->nItems = params.nSettingItems = n;
	if (n>0) {
		if (n>MAX_SETTINGS) n = MAX_SETTINGS;
		PSSettingItem *pItem = params.settingItems;
		ExMenuItem *pMenuItem = pExMenu->items;
		for (i=0;i<n;i++) {
			if (pItem->pAlias != NULL) free(pItem->pAlias);
			if (pItem->pHostName != NULL) free(pItem->pHostName);
			if (pItem->pPortName != NULL) free(pItem->pPortName);
			GetLVEntry(i, pItem);
			////////////
			if (pItem->pAlias[0] != 0)
				strncpy(pMenuItem->DisplayName, pItem->pAlias, MAX_PATH);
			else
				_snprintf(pMenuItem->DisplayName, MAX_PATH, "%s - %s",
					pItem->pHostName,
					pItem->pPortName);
			if (strcmp(pItem->pHostName, params.RemoteHostName) == 0 &&
				strcmp(pItem->pPortName, params.RemotePortName) == 0)
				pMenuItem->state = EX_MS_CHECKED;
			else
				pMenuItem->state = EX_MS_NORMAL;

			//
			pItem++;
			pMenuItem++;
		}
	}
	//
	////////////////////////////////////
	params.bAutoCheck = ButtonGetCheck(GetDlgItem(IDC_AUTOCHECK));
	////////////////////////////////////
	//////////////////////////////////////////////////////////
	pMyTransport->PutParameters(&params);
	pMyTransport->SaveSetting();

	// fix tray icon if needed
	if (!params.bAutoCheck) {
		TrayData tData;
		tData.iconKind = TI_NORMAL;
		pMyTransport->MgrUIControl(UIC_TRAY_CHANGEICON, &tData);
	}
}

void TcpShareDlg::CancelChange()
{
	SetDlgItemData();
}

void TcpShareDlg::ExMenuSelected(int iMenu)
{
	if (iMenu < 0 || iMenu >= params.nSettingItems)
		return;
	// set check to selected item
	int i;
	for (i=0;i<params.nSettingItems;i++) {
		if (i == iMenu)
			pExMenu->items[i].state = EX_MS_CHECKED;
		else
			pExMenu->items[i].state = EX_MS_NORMAL;
	}
	////
	int rpIndex = atoi(params.settingItems[iMenu].pPortName + 3);
	if (rpIndex < 1)
		return;
	rpIndex--;
	ComboBoxSetCurSel(GetDlgItem(IDC_COMBO_REMOTEP), rpIndex);
	SetWindowText(GetDlgItem(IDC_COMBO_REMOTEM), params.settingItems[iMenu].pHostName);

	//////////////////////////////////////////////////////////
	pMyTransport->PutParameters(&params);
	pMyTransport->SaveSetting();

}
////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void TcpShareDlg::SetDlgItemData()
{
	int i;
	////////////////////////////////////
	ComboBoxResetContent(GetDlgItem(IDC_COMBO_REMOTEM));
	SetWindowText(GetDlgItem(IDC_COMBO_REMOTEM), params.RemoteHostName);
	for (i=0;i<NUM_HOSTNAMES;i++) {
		if (params.RemoteHostNames[i][0] != (char) 0)
			ComboBoxAddString(GetDlgItem(IDC_COMBO_REMOTEM), params.RemoteHostNames[i]);
	}
	////////////////////////////////////
	int iCurSel;
	char **pPortNameList = pMyTransport->getPortNameList();
	for (i=0;i<pMyTransport->getNumPortNames();i++) {
		if (strcmp(pPortNameList[i], params.RemotePortName) == 0) {
			iCurSel = i;
		}
	}
	ComboBoxSetCurSel(GetDlgItem(IDC_COMBO_REMOTEP), iCurSel);
	////////////////////////////////////
	ButtonSetCheck(GetDlgItem(IDC_AUTOCHECK), 
		(params.bAutoCheck ? 1 : 0));
	////////////////////////////////////
	ListView_Clear(GetDlgItem(IDC_SETTINGLIST));
	PSSettingItem *pItem = params.settingItems;
	for (i=0;i<params.nSettingItems;i++) {
		AddLVEntry(pItem);
		pItem++;
	}

}

void TcpShareDlg::Selected(int bFlag)
{
	bSelected = bFlag;
	if (bFlag == TRUE) {
		pMyTransport->MgrUIControl(UIC_MENUARRAY_SET, pExMenu);
	}
}

///////////////////////////////////////////////////////////
// UDPCallback methods
void TcpShareDlg::RConnectable(int bFlag)
{
	// if true, it "may" connectable
	// if false, it is not connectable
	bConnectable = bFlag;
}

void TcpShareDlg::RServerEntry(char *serverName)
{
	HWND hCB = GetDlgItem(IDC_COMBO_REMOTEM);
	tcnt_GETSERVER = 0;
	if (CB_ERR == ComboBoxFindString(hCB, serverName)) {
		SetWindowText(hCB, serverName);
		SetModified(TRUE);
	}
}

void TcpShareDlg::RStatus(char *serverName, char *clientName, char *portName)
{
	int iLine;
	char tmpBuf[MAX_PATH];
	_snprintf(tmpBuf, MAX_PATH, "\t%s\t%s", 
		(clientName != NULL ? clientName : ""),
		(portName != NULL ? portName : ""));
	iLine = SearchTextLineResultEdit(serverName);
	if (iLine != -1) {
		PrintLineToResultEdit(tmpBuf, iLine + 1);
	} else {
		PrintLineToResultEdit(serverName);
		PrintLineToResultEdit(tmpBuf);
	}
}

void TcpShareDlg::DoDDE(XS_DDE *pPack)
{
	static char resultBuf[MAX_PATH]; // 
	int vInt;
	if (pPack->command == XS_DDE_EXECUTE) {
		char *pCommandString;
		char *pCommand, *pArg1, *pArg2;
		resultBuf[0] = (char) 0;
		pCommandString = strdup(pPack->ddeData.pExecString);
		pCommand = strtok(pCommandString, ":");
		pArg1 = strtok(NULL, ", ");
		if (pArg1 != NULL)
			pArg2 = strtok(NULL, ", ");
		else
			pArg2 = NULL;
		if (pArg1 != NULL && pArg1[0] == ',') {
			pArg2 = &pArg1[1];
			pArg1 = NULL;
		}
		if (strcmp(pCommand, SETPEERCONFIG) == 0) {
			if (pArg1 != NULL) {
				strncpy(params.RemoteHostName, pArg1, MAX_PATH - 1);
			}
			if (pArg2 != NULL) {
				strncpy(params.RemotePortName, pArg2, 31);
			}
			_snprintf(resultBuf, MAX_PATH, "%s,%s",
							params.RemoteHostName, params.RemotePortName);
			SetDlgItemData();
			ApplyChange();
		} else
		if (strcmp(pCommand, SELECTFROMLIST) == 0) {
			if (pArg1 == NULL) {
				sprintf(resultBuf, "%d", params.nSettingItems);
			} else
			if ((vInt = atoi(pArg1)) >= 0 && vInt < params.nSettingItems) {
				ExMenuSelected(vInt);
				_snprintf(resultBuf, MAX_PATH, "%s,%s",
							params.RemoteHostName, params.RemotePortName);

			} else {
				strcpy(resultBuf, "Bad index");
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

