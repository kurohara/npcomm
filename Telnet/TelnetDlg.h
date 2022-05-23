/***********************************************************************
    TelnetDlg.h
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
#ifndef _TELNETDLG_H
#define _TELNETDLG_H
#include <Windows.h>
#include "resource.h"
#include "DlgClass.h"
#include "UICtrl.h"
#include "Params.h"

// means callback points transport
class XsUiIntf
{
public:
	virtual void PutParameters(TelnetParams *newParams) = 0;
	virtual void GetParameters(TelnetParams *newParams) = 0;
	virtual void SaveSetting() = 0;
	virtual void LoadSetting() = 0;
	virtual int MgrUIControl(int Command, void *pData) = 0;
	virtual int getTransportId() = 0;
};

class TelnetDlg : public aDialog
{
public:
	TelnetDlg(HWND parent = NULL);
	~TelnetDlg();
	enum { IDD = IDD_TELNET_SETTING} ;
	virtual int OnCommand(WPARAM wParam, LPARAM lParam);

// Control Command handlers
	void InitDialog(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnDblClick(WPARAM wParam, LPARAM lParam);
	void OnTimer(WPARAM wParam, LPARAM lParam);
	void OnSockNotify(WPARAM wParam, LPARAM lParam);

	void CONNECTONDIAL_Clicked(WPARAM wParam, LPARAM lParam);
	void USE_PROXY_Clicked(WPARAM wParam, LPARAM lParam);
	void KICKRAS_Clicked(WPARAM wParam, LPARAM lParam);
#if 0 // if you need to handle each of controls, you may need these methods.
	void PROTOCOL1_Clicked(WPARAM wParam, LPARAM lParam);
	void PROTOCOL2_Clicked(WPARAM wParam, LPARAM lParam);
	void PROTOCOL3_Clicked(WPARAM wParam, LPARAM lParam);

	void DIAL_COMMAND1_EditChange(WPARAM wParam, LPARAM lParam);
	void DIAL_COMMAND2_EditChange(WPARAM wParam, LPARAM lParam);
	void DIAL_COMMAND3_EditChange(WPARAM wParam, LPARAM lParam);
	void DESTINATION1_EditChange(WPARAM wParam, LPARAM lParam);
	void DESTINATION2_EditChange(WPARAM wParam, LPARAM lParam);
	void DESTINATION3_EditChange(WPARAM wParam, LPARAM lParam);
	void PROXY_SERVER_EditChange(WPARAM wParam, LPARAM lParam);
	void PROXY_PORT_EditChange(WPARAM wParam, LPARAM lParam);

	void RASENTRIES_SelChange(WPARAM wParam, LPARAM lParam);
#endif
	//
	XsUiIntf *pMyTransport;
// dll local interfaces
	void ExMenuSelected(int iMenu);
	void ApplyChange();
	void CancelChange();

// data utilities
	void SetupSheet();
	TelnetParams params;
	ExMenuArray *pExMenu;

};


#endif _TELNETDLG_H
