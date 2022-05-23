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
#include "DlgClass.h"

#define MYWM_NOTIFYICON		(WM_APP+100)
#define MYWM_DEFERWORK		(WM_APP+101)
#include "resource.h"
#include "instdrv.h"

class MyDialog : public aDialog
{
public:
	MyDialog(HWND parent = NULL);
	~MyDialog();
	enum { IDD = IDD_INSTNP_DIALOG };
	drvInfo dInfo;
	int bNpcommRunning;
	int bInstService;
	int bInstMulti;
	int bDriverInstalled;
	int bDriverRunning;
	int bDriverAutoStart;
	int bDriverExist;
	int iFirstPcomm;
	char driverPath[MAX_PATH];
	//
	int iCurrentNpcomm;
	int bService[3];
	char arguments[3][MAX_PATH];
	int bNpcommExist;
// Command handlers
	void InitDialog(WPARAM wParam, LPARAM lParam);
	void OnClose(WPARAM wParam, LPARAM lParam);
	//
	void OnPPortNameSelChange(WPARAM wParam, LPARAM lParam);
	void OnPPort1NameSelChange(WPARAM wParam, LPARAM lParam);
	void OnInstallDriver(WPARAM wParam, LPARAM lParam);
	void OnUninstallDriver(WPARAM wParam, LPARAM lParam);
	void OnStartDriver(WPARAM wParam, LPARAM lParam);
	void OnNpcommClicked(WPARAM wParam, LPARAM lParam);
	void OnNpcomm1Clicked(WPARAM wParam, LPARAM lParam);
	void OnNpcomm2Clicked(WPARAM wParam, LPARAM lParam);
	void OnInstallService(WPARAM wParam, LPARAM lParam);
	void OnUninstallService(WPARAM wParam, LPARAM lParam);
	//
	void EnableMulti();
	void UpdateDriverPortion();
	void UpdateServicePortion();
private:
};

