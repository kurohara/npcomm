/***********************************************************************
    TelnetDlg.cpp
    dialog box interface implementation for TELNET module.

	Copyright (C) 1999-2003 by Hiroyoshi Kurohara
		All Rights Reserved

  	This file is part of TELNET XP module(transport DLL for NPCOMM).

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

#include "Telnet.h"
#include "TelnetDlg.h"

extern HINSTANCE gMyHandle;

#define MYWM_SOCKET (WM_USER+100)

///////////////////////////////////////////////////////////////////////
static MessageMap myMap[] = {
	MESSAGE_HANDLER(TelnetDlg, WM_INITDIALOG, InitDialog),
#if 0 // does not needed
	MESSAGE_HANDLER(TelnetDlg, WM_NOTIFY, OnNotify),
	MESSAGE_HANDLER(TelnetDlg, WM_LBUTTONDBLCLK, OnDblClick),
	MESSAGE_HANDLER(TelnetDlg, WM_TIMER, OnTimer),
	MESSAGE_HANDLER(TelnetDlg, MYWM_SOCKET, OnSockNotify),
#endif
	// check boxes
	COMMAND_HANDLER(TelnetDlg, BN_CLICKED, IDC_CONNECTONDIAL, CONNECTONDIAL_Clicked),
	COMMAND_HANDLER(TelnetDlg, BN_CLICKED, IDC_USE_PROXY, USE_PROXY_Clicked),
	COMMAND_HANDLER(TelnetDlg, BN_CLICKED, IDC_KICKRAS, KICKRAS_Clicked),
#if 0 // if you need to handle each of messages, you may need these entries
	COMMAND_HANDLER(TelnetDlg, BN_CLICKED, IDC_PROTOCOL1, PROTOCOL1_Clicked),
	COMMAND_HANDLER(TelnetDlg, BN_CLICKED, IDC_PROTOCOL2, PROTOCOL2_Clicked),
	COMMAND_HANDLER(TelnetDlg, BN_CLICKED, IDC_PROTOCOL3, PROTOCOL3_Clicked),
	//
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_DIAL_COMMAND1, DIAL_COMMAND1_EditChange),
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_DIAL_COMMAND2, DIAL_COMMAND2_EditChange),
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_DIAL_COMMAND3, DIAL_COMMAND3_EditChange),
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_DESTINATION1, DESTINATION1_EditChange),
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_DESTINATION2, DESTINATION2_EditChange),
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_DESTINATION3, DESTINATION3_EditChange),
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_PROXY_SERVER, PROXY_SERVER_EditChange),
	COMMAND_HANDLER(TelnetDlg, EN_UPDATE, IDC_PROXY_PORT, PROXY_PORT_EditChange),
	COMMAND_HANDLER(TelnetDlg, CBN_SELCHANGE, IDC_RASENTRIES, RASENTRIES_SelChange),
#endif // 0
	//
	{ 0, 0, 0, NULL }
} ;
///////////////////////////////////////////////////////////////////////
TelnetDlg::TelnetDlg(HWND parent)
: aDialog(TelnetDlg::IDD, parent)
{
	////////////////////////////////////////////
	pMesMap = (void *) myMap;
	////////////////////////////////////////////

}

TelnetDlg::~TelnetDlg()
{

}

void TelnetDlg::SetupSheet()
{
	char tmpStrBuf[MAX_PATH];

	// auto connect settings
	ButtonSetCheck(GetDlgItem(IDC_CONNECTONDIAL), params.bConnectOnDial);
	SetWindowText(GetDlgItem(IDC_DIAL_COMMAND1), params.dialParams[0].DialCommand);
	SetWindowText(GetDlgItem(IDC_DIAL_COMMAND2), params.dialParams[1].DialCommand);
	SetWindowText(GetDlgItem(IDC_DIAL_COMMAND3), params.dialParams[2].DialCommand);
	SetWindowText(GetDlgItem(IDC_DESTINATION1), params.dialParams[0].Destination);
	SetWindowText(GetDlgItem(IDC_DESTINATION2), params.dialParams[1].Destination);
	SetWindowText(GetDlgItem(IDC_DESTINATION3), params.dialParams[2].Destination);
	itoa(params.dialParams[0].IPPort, tmpStrBuf, 10);
	SetWindowText(GetDlgItem(IDC_IPPORT1), tmpStrBuf);
	itoa(params.dialParams[1].IPPort, tmpStrBuf, 10);
	SetWindowText(GetDlgItem(IDC_IPPORT2), tmpStrBuf);
	itoa(params.dialParams[2].IPPort, tmpStrBuf, 10);
	SetWindowText(GetDlgItem(IDC_IPPORT3), tmpStrBuf);
	// proxy settings
	ButtonSetCheck(GetDlgItem(IDC_USE_PROXY), params.bUseProxy);
	SetWindowText(GetDlgItem(IDC_PROXY_SERVER), params.proxParams.ProxyServerName);
	itoa(params.proxParams.ProxyPortNum, tmpStrBuf, 10);
	SetWindowText(GetDlgItem(IDC_PROXY_PORT), tmpStrBuf);
	// ras setting
	ButtonSetCheck(GetDlgItem(IDC_KICKRAS), params.rasParams.bKickRas);
	int cbIndex = ComboBoxFindString(GetDlgItem(IDC_RASENTRIES), params.rasParams.RasEntryName);
	if (cbIndex != CB_ERR)
		ComboBoxSetCurSel(GetDlgItem(IDC_RASENTRIES), cbIndex);
	else
		ComboBoxSetCurSel(GetDlgItem(IDC_RASENTRIES), 0);

	// enabel/disable controls
	CONNECTONDIAL_Clicked(0, 0);
	USE_PROXY_Clicked(0, 0);
	KICKRAS_Clicked(0, 0);

}

#include <ras.h>
void TelnetDlg::InitDialog(WPARAM wParam, LPARAM lParam)
{

	SetWindowText(GetDlgItem(IDC_DIALOGLABEL), XPM_SIGNATURE);

	pMyTransport->GetParameters(&params);
	//
	// for RAS setting
	RASENTRYNAME rasEntries[30];
	unsigned long cSize, rSize;
	int i;
	HINSTANCE hRasApi;
	int (_stdcall * pRasEnumEntries)(char *, char *, RASENTRYNAME *, unsigned long *, unsigned long *);
	HWND hCB = GetDlgItem(IDC_RASENTRIES);

	hRasApi = LoadLibrary("RASAPI32.DLL");
	if (hRasApi != NULL) {
		pRasEnumEntries =
			(int (_stdcall * )(char *, char *, RASENTRYNAME *, unsigned long *, unsigned long *))
			GetProcAddress(hRasApi, "RasEnumEntriesA");
		rasEntries[0].dwSize = sizeof (RASENTRYNAME);
		cSize = sizeof (rasEntries);
		if (pRasEnumEntries != NULL) {
			if ((*pRasEnumEntries)(NULL, NULL, rasEntries, &cSize, &rSize) == 0) {
				for (i=0;i<rSize;i++) {
					ComboBoxAddString(hCB, rasEntries[i].szEntryName);
				}
			}
		}
		FreeLibrary(hRasApi);
	}
	//
	SetupSheet();
	////////////////////////////////////
}

int TelnetDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) == BN_CLICKED ||
		HIWORD(wParam) == EN_UPDATE ||
		HIWORD(wParam) == CBN_SELCHANGE) {
		pMyTransport->MgrUIControl(UIC_SHEET_CHANGED, NULL);
	}
	return aDialog::OnCommand(wParam, lParam);
}

void TelnetDlg::OnNotify(WPARAM wParam, LPARAM lParam)
{

}

void TelnetDlg::OnDblClick(WPARAM wParam, LPARAM lParam)
{
}

void TelnetDlg::OnTimer(WPARAM wParam, LPARAM lParam)
{

}

void TelnetDlg::OnSockNotify(WPARAM wParam, LPARAM lParam)
{

}

///////////////////////////////////////////////////////////////////////
// Control message handlers
#define ENABLE_DIAL_CONTROLS(bEnable) (\
	EnableWindow(GetDlgItem(IDC_DIAL_COMMAND1), bEnable), \
	EnableWindow(GetDlgItem(IDC_DIAL_COMMAND2), bEnable), \
	EnableWindow(GetDlgItem(IDC_DIAL_COMMAND3), bEnable), \
	EnableWindow(GetDlgItem(IDC_DESTINATION1), bEnable), \
	EnableWindow(GetDlgItem(IDC_DESTINATION2), bEnable), \
	EnableWindow(GetDlgItem(IDC_DESTINATION3), bEnable), \
	EnableWindow(GetDlgItem(IDC_IPPORT1), bEnable), \
	EnableWindow(GetDlgItem(IDC_IPPORT2), bEnable), \
	EnableWindow(GetDlgItem(IDC_IPPORT3), bEnable) )

void TelnetDlg::CONNECTONDIAL_Clicked(WPARAM wParam, LPARAM lParam)
{
	if (ButtonGetCheck(GetDlgItem(IDC_CONNECTONDIAL))) {
		ENABLE_DIAL_CONTROLS(TRUE);
	} else {
		ENABLE_DIAL_CONTROLS(FALSE);
	}
}

void TelnetDlg::USE_PROXY_Clicked(WPARAM wParam, LPARAM lParam)
{
	if (ButtonGetCheck(GetDlgItem(IDC_USE_PROXY))) {
		EnableWindow(GetDlgItem(IDC_PROXY_SERVER), TRUE);
		EnableWindow(GetDlgItem(IDC_PROXY_PORT), TRUE);
	} else {
		EnableWindow(GetDlgItem(IDC_PROXY_SERVER), FALSE);
		EnableWindow(GetDlgItem(IDC_PROXY_PORT), FALSE);
	}
}

void TelnetDlg::KICKRAS_Clicked(WPARAM wParam, LPARAM lParam)
{
	if (ButtonGetCheck(GetDlgItem(IDC_KICKRAS))) {
		EnableWindow(GetDlgItem(IDC_RASENTRIES), TRUE);
	} else {
		EnableWindow(GetDlgItem(IDC_RASENTRIES), FALSE);
	}
}

///////////////////////////////////////////////////////////////////////
void TelnetDlg::ApplyChange()
{
	char tmpStrBuf[MAX_PATH];

	// auto connect settings
	params.bConnectOnDial = ButtonGetCheck(GetDlgItem(IDC_CONNECTONDIAL));
	GetWindowText(GetDlgItem(IDC_DIAL_COMMAND1), params.dialParams[0].DialCommand, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_DIAL_COMMAND2), params.dialParams[1].DialCommand, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_DIAL_COMMAND3), params.dialParams[2].DialCommand, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_DESTINATION1), params.dialParams[0].Destination, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_DESTINATION2), params.dialParams[1].Destination, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_DESTINATION3), params.dialParams[2].Destination, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_IPPORT1), tmpStrBuf, MAX_PATH);
	params.dialParams[0].IPPort = atoi(tmpStrBuf);
	GetWindowText(GetDlgItem(IDC_IPPORT2), tmpStrBuf, MAX_PATH);
	params.dialParams[1].IPPort = atoi(tmpStrBuf);
	GetWindowText(GetDlgItem(IDC_IPPORT3), tmpStrBuf, MAX_PATH);
	params.dialParams[2].IPPort = atoi(tmpStrBuf);
	// proxy settings
	params.bUseProxy = ButtonGetCheck(GetDlgItem(IDC_USE_PROXY));
	GetWindowText(GetDlgItem(IDC_PROXY_SERVER), params.proxParams.ProxyServerName, MAX_PATH);
	GetWindowText(GetDlgItem(IDC_PROXY_PORT), tmpStrBuf, MAX_PATH);
	params.proxParams.ProxyPortNum = atoi(tmpStrBuf);
	// ras setting
	params.rasParams.bKickRas = ButtonGetCheck(GetDlgItem(IDC_KICKRAS));
	GetWindowText(GetDlgItem(IDC_RASENTRIES), params.rasParams.RasEntryName, MAX_PATH);

	//////////////////////////////////////////////////////////
	pMyTransport->PutParameters(&params);
	pMyTransport->SaveSetting();
}

void TelnetDlg::CancelChange()
{
	SetupSheet();
}

void TelnetDlg::ExMenuSelected(int iMenu)
{
}

///////////////////////////////////////////////////////////////////////
