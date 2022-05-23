/***********************************************************************
    TcpShDlg.h
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
#ifndef _TCPSHDLG_H
#define _TCPSHDLG_H

#include "resource.h"
#include "DlgClass.h"
#include "UICtrl.h"
#include "Sock.h"
#include "Params.h"

// for DDE
// topic name for this transport is automatically set to
// the name of transport.
// command strings for TcpShare topic
#define SELECTFROMLIST "SelectFromList"
#define SETPEERCONFIG  "SetPeerConfig"
//
// for REQUEST transaction
// item
#define RESULT_ITEM "RESULT_STRING"
//

// means callback points transport
class XsUiIntf
{
public:
	virtual int ConnectionTest(int bTouch, int *mStat) = 0;
	virtual int getNumPortNames() = 0;
	virtual char **getPortNameList() = 0;
	virtual void PutParameters(TcpShareParams *newParams) = 0;
	virtual void GetParameters(TcpShareParams *newParams) = 0;
	virtual void SaveSetting() = 0;
	virtual void LoadSetting() = 0;
	virtual int isConnected() = 0;
	virtual int MgrUIControl(int Command, void *pData) = 0;
	virtual int getTransportId() = 0;
};

class TcpShareDlg : public aDialog, public UDPCallback
{
public:
	TcpShareDlg(HWND parent = NULL);
	~TcpShareDlg();
	enum { IDD = IDD_PORTSRV_SETTING} ;
	virtual int OnCommand(WPARAM wParam, LPARAM lParam);

// Command handlers
	void InitDialog(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnTimer(WPARAM wParam, LPARAM lParam);
	void OnSockNotify(WPARAM wParam, LPARAM lParam);
	void GETSERVER_Clicked(WPARAM wParam, LPARAM lParam);
	void ADDTOLIST_Clicked(WPARAM wParam, LPARAM lParam);
	void REMOVEFROMLIST_Clicked(WPARAM wParam, LPARAM lParam);
	void GETSERVERSTATUS_Clicked(WPARAM wParam, LPARAM lParam);
	void PINGER_Clicked(WPARAM wParam, LPARAM lParam);
	void AUTOCHECK_Clicked(WPARAM wParam, LPARAM lParam);
	void DOTOUCH_Clicked(WPARAM wParam, LPARAM lParam);
	void REMOTEM_SelChange(WPARAM wParam, LPARAM lParam);
	void REMOTEM_EditChange(WPARAM wParam, LPARAM lParam);
	void REMOTEP_SelChange(WPARAM wParam, LPARAM lParam);

// utilities for window controls
	void SetupListview(void);
	void AddLVEntry(PSSettingItem *pItem);
	void GetLVEntry(int index, PSSettingItem *pItem);
	void PrintToResultEdit(char *pText,
									 int bAppend = FALSE,
									 int iPos = -1);
	void PrintLineToResultEdit(char *pText, int iLine = -1);
	int SearchTextLineResultEdit(char *pText);
	void OnDblClickControl();
	void AddHostEntryToList(char *pRemoteHostName);

// dll local interfaces
	void ExMenuSelected(int iMenu);
	void ApplyChange();
	void CancelChange();

// data utilities
	void SetDlgItemData();
	void Selected(int bFlag);
	int bSelected;// TRUE if this transport is selected, reffered by AutoCheck

	UDPSock uSock;
	int bConnectable;

	//
	XsUiIntf *pMyTransport;
	//
	TcpShareParams params;
	ExMenuArray *pExMenu;
// for timer proc
	int tcnt_GETSERVER;

	// UDPCallback methods
	void RConnectable(int bConnectable);
	void RServerEntry(char *serverName);
	void RStatus(char *serverName, char *clientName, char *portName);

	// for DDE
	void DoDDE(XS_DDE *pDDE);
};

#endif _TCPSHDLG_H
