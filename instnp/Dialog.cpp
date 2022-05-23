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
#include <stdio.h>

#include "Dialog.h"

#define MYWM_NOTIFYICON		(WM_APP+100)
#define IDC_DUMM                        1200
///////////////////////////////////////////////////////////////////
extern int CheckNpcommRunning();
extern int CheckNpcommExist();
extern int CheckDriverExist();
extern int GetServiceStatus(int index, int *bRunning, char *pPath, int *bAuto);
extern int NTServiceRemove(int index);
extern int NTServiceInstall(int index, char *pExArguments);
extern int AppMessageBox(unsigned int resid, unsigned int mask);

static MessageMap mainMap[] = {
	MESSAGE_HANDLER(MyDialog, WM_INITDIALOG, InitDialog),
	MESSAGE_HANDLER(MyDialog, WM_CLOSE, OnClose),
	COMMAND_HANDLER(MyDialog, CBN_SELCHANGE, IDC_PPORTNAME, OnPPortNameSelChange),
	COMMAND_HANDLER(MyDialog, CBN_SELCHANGE, IDC_PPORTNAME1, OnPPort1NameSelChange),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_INSTALL, OnInstallDriver),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_DEINSTALL, OnUninstallDriver),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_START, OnStartDriver),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_NPCOMM, OnNpcommClicked),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_NPCOMM1, OnNpcomm1Clicked),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_NPCOMM2, OnNpcomm2Clicked),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_INSTALLSERVICE, OnInstallService),
	COMMAND_HANDLER(MyDialog, BN_CLICKED, IDC_DEINSTALLSERVICE, OnUninstallService),
	{ 0, 0, 0, NULL }

} ;

MyDialog::MyDialog(HWND parent)
: aDialog(MyDialog::IDD, parent)
{
	pMesMap = (void *) mainMap;
}

MyDialog::~MyDialog()
{

}

void MyDialog::UpdateDriverPortion()
{
	int bFlag;
	int i;
	char comName[3][64];
	ComboBoxResetContent(GetDlgItem(IDC_PPORTNAME));
	ComboBoxResetContent(GetDlgItem(IDC_PPORTNAME1));
	ComboBoxResetContent(GetDlgItem(IDC_PPORTNAME2));
	for (i=0;i<3;i++) {
		if (dInfo.nPCOMM > i)
			sprintf(comName[i], "COM%d", dInfo.pinfo[i].portNum + 1);
		else
			strcpy(comName[i], "NONE");
	}

	ButtonSetCheck(GetDlgItem(IDC_AUTOSTART), bDriverAutoStart);

	if (bDriverInstalled) {
		HWND hCB;
		hCB = GetDlgItem(IDC_PPORTNAME);
		ComboBoxAddString(hCB, comName[0]);
		ComboBoxSetCurSel(hCB, 0);
		hCB = GetDlgItem(IDC_PPORTNAME1);
		ComboBoxAddString(hCB, comName[1]);
		ComboBoxSetCurSel(hCB, 0);
		hCB = GetDlgItem(IDC_PPORTNAME2);
		ComboBoxAddString(hCB, comName[2]);
		ComboBoxSetCurSel(hCB, 0);
		//
		EnableWindow(GetDlgItem(IDC_AUTOSTART), FALSE);
		EnableWindow(GetDlgItem(IDC_INSTALL), FALSE);
		EnableWindow(GetDlgItem(IDC_DEINSTALL), TRUE);
		if (bDriverRunning)
			EnableWindow(GetDlgItem(IDC_START), FALSE);
		else
			EnableWindow(GetDlgItem(IDC_START), TRUE);
		if (dInfo.nPCOMM > 1) {
			EnableWindow(GetDlgItem(IDC_PPORTNAME2), TRUE);
		} else {
			EnableWindow(GetDlgItem(IDC_PPORTNAME2), FALSE);
		}
	} else {
		char tmpName[64];
		HWND hCB;
		int cbIndex;
		int iNextPcomm;
		// 0
		hCB = GetDlgItem(IDC_PPORTNAME);
		for (i = iFirstPcomm;i < 10;i++) {
			sprintf(tmpName, "COM%d", i);
			ComboBoxAddString(hCB, tmpName);
		}
		cbIndex = ComboBoxFindString(hCB, comName[0]);
		ComboBoxSetCurSel(hCB, cbIndex == CB_ERR ? 0 : cbIndex);
		// 1
		if (dInfo.nPCOMM > 0) {
			iNextPcomm = dInfo.pinfo[0].portNum + 2;
		}
		hCB = GetDlgItem(IDC_PPORTNAME1);
		ComboBoxAddString(hCB, "NONE");
		for (i = iNextPcomm;i < 10;i++) {
			sprintf(tmpName, "COM%d", i);
			ComboBoxAddString(hCB, tmpName);
		}
		cbIndex = ComboBoxFindString(hCB, comName[1]);
		ComboBoxSetCurSel(hCB, cbIndex == CB_ERR ? 0 : cbIndex);
		// 2
		if (dInfo.nPCOMM > 1) {
			iNextPcomm = dInfo.pinfo[1].portNum + 2;
			hCB = GetDlgItem(IDC_PPORTNAME2);
			ComboBoxAddString(hCB, "NONE");
			for (i = iNextPcomm;i < 10;i++) {
				sprintf(tmpName, "COM%d", i);
				ComboBoxAddString(hCB, tmpName);
			}
			cbIndex = ComboBoxFindString(hCB, comName[2]);
			ComboBoxSetCurSel(hCB, cbIndex == CB_ERR ? 0 : cbIndex);
			EnableWindow(hCB, TRUE);
		} else {
			EnableWindow(GetDlgItem(IDC_PPORTNAME2), FALSE);
		}
		////////
		EnableWindow(GetDlgItem(IDC_AUTOSTART), TRUE);
		EnableWindow(GetDlgItem(IDC_INSTALL), TRUE);
		EnableWindow(GetDlgItem(IDC_DEINSTALL), FALSE);
		EnableWindow(GetDlgItem(IDC_START), FALSE);
	}
	if (bNpcommRunning) {
		SetWindowText(GetDlgItem(IDC_DRIVERFRAME), "P-Serial driver installation requires all npcomm stopped");
		bFlag = FALSE;
		EnableWindow(GetDlgItem(IDC_DRIVERFRAME), bFlag);
		EnableWindow(GetDlgItem(IDC_PNAME), bFlag);
		EnableWindow(GetDlgItem(IDC_PNAME1), bFlag);
		EnableWindow(GetDlgItem(IDC_PNAME2), bFlag);
		EnableWindow(GetDlgItem(IDC_PPORTNAME), bFlag);
		EnableWindow(GetDlgItem(IDC_PPORTNAME1), bFlag);
		EnableWindow(GetDlgItem(IDC_PPORTNAME2), bFlag);
		EnableWindow(GetDlgItem(IDC_AUTOSTART), bFlag);
		EnableWindow(GetDlgItem(IDC_INSTALL), bFlag);
		EnableWindow(GetDlgItem(IDC_DEINSTALL), bFlag);
		EnableWindow(GetDlgItem(IDC_START), bFlag);
	} else {
		SetWindowText(GetDlgItem(IDC_DRIVERFRAME), "P-Serial driver");
		bFlag = TRUE;
		EnableWindow(GetDlgItem(IDC_DRIVERFRAME), bFlag);
		EnableWindow(GetDlgItem(IDC_PNAME), bFlag);
		EnableWindow(GetDlgItem(IDC_PNAME1), bFlag);
		EnableWindow(GetDlgItem(IDC_PNAME2), bFlag);
		EnableWindow(GetDlgItem(IDC_PPORTNAME), bFlag);
		EnableWindow(GetDlgItem(IDC_PPORTNAME1), bFlag);
	}

}

void MyDialog::UpdateServicePortion()
{

	int bInstallable =
		(! bService[iCurrentNpcomm]) &&
		iCurrentNpcomm < dInfo.nPCOMM &&
		bDriverInstalled &&
		bDriverAutoStart;

	EnableWindow(GetDlgItem(IDC_DEINSTALLSERVICE), bService[iCurrentNpcomm]);
	EnableWindow(GetDlgItem(IDC_INSTALLSERVICE), bInstallable);

	ButtonSetCheck(GetDlgItem(IDC_ISSERVICE), bService[iCurrentNpcomm]);
	SetWindowText(GetDlgItem(IDC_EXTRAARGUMENTS), arguments[iCurrentNpcomm]);
	EnableWindow(GetDlgItem(IDC_EXTRAARGUMENTS), bInstallable);
}

void MyDialog::InitDialog(WPARAM wParam, LPARAM lParam)
{
	int i;
	if (!(bNpcommExist = CheckNpcommExist()))
		AppMessageBox(IDS_NONPCOMM, MB_OK | MB_ICONWARNING);
	if (!(bDriverExist = CheckDriverExist()))
		AppMessageBox(IDS_NODRIVER, MB_OK | MB_ICONWARNING);

	SetDriverPath(NULL);

	iFirstPcomm = GetLastComId() + 1;
	memset((char *) &dInfo, 0, sizeof (dInfo));
	for (i=0;i<3;i++) {
		sprintf(dInfo.pinfo[i].pcomName, "PSerial32%d", i);
		sprintf(dInfo.pinfo[i].dpcomName, "PCOMM32%d", i);
	}
	dInfo.nPCOMM = 1;
	dInfo.pinfo[0].portNum = iFirstPcomm - 1;
	GetParameters(&dInfo);
	bDriverRunning = FALSE;
	bDriverAutoStart = FALSE;
	bDriverInstalled = GetDriverStatus(&bDriverRunning, driverPath, &bDriverAutoStart);
	bNpcommRunning = CheckNpcommRunning();
	UpdateDriverPortion();

	//
	iCurrentNpcomm = 0;
	ButtonSetCheck(GetDlgItem(IDC_NPCOMM), TRUE);
	int bRunning;
	char servicePath[MAX_PATH];
	char *pArgv;
	int bAutoStart;
	for (i=0;i<3;i++) {
		bService[i] = GetServiceStatus(i, &bRunning, servicePath, &bAutoStart);
		arguments[i][0] = (char) 0;
		pArgv = strtok(servicePath, " ");
		pArgv = strtok(NULL, " ");
		while ((pArgv = strtok(NULL, " "))) {
			if (pArgv[1] != 'd') {
				strcat(arguments[i], " ");
				strcat(arguments[i], pArgv);
			}
		}
	}
	UpdateServicePortion();
}


void MyDialog::OnClose(WPARAM wParam, LPARAM lParam)
{
	EndDialog(hDlg, 0);
	PostQuitMessage(TRUE);
}

void MyDialog::EnableMulti()
{
	if (bInstService) {
		EnableWindow(GetDlgItem(IDC_SERVICEFRAME), TRUE);
		EnableWindow(GetDlgItem(IDC_NPCOMM), TRUE);
		EnableWindow(GetDlgItem(IDC_ISSERVICE), TRUE);
		EnableWindow(GetDlgItem(IDC_ARGLABEL), TRUE);
		EnableWindow(GetDlgItem(IDC_EXTRAARGUMENTS), TRUE);
		EnableWindow(GetDlgItem(IDC_INSTALLSERVICE), TRUE);
		if (bInstMulti) {
			EnableWindow(GetDlgItem(IDC_NPCOMM1), TRUE);
			EnableWindow(GetDlgItem(IDC_NPCOMM2), TRUE);
		}
	}
	if (bInstMulti) {
		EnableWindow(GetDlgItem(IDC_PNAME1), TRUE);
		EnableWindow(GetDlgItem(IDC_PNAME2), TRUE);
		EnableWindow(GetDlgItem(IDC_PPORTNAME1), TRUE);
		EnableWindow(GetDlgItem(IDC_PPORTNAME2), TRUE);
	}
}

void MyDialog::OnPPortNameSelChange(WPARAM wParam, LPARAM lParam)
{
	char comName[64];
	GetWindowText(GetDlgItem(IDC_PPORTNAME), comName, 64);
	dInfo.pinfo[0].portNum = atoi(&comName[3]) - 1;
	if (dInfo.pinfo[0].portNum >= 7) {
		if (dInfo.nPCOMM > 2)
			dInfo.nPCOMM = 2;
	}
	UpdateDriverPortion();
}

void MyDialog::OnPPort1NameSelChange(WPARAM wParam, LPARAM lParam)
{
	char comName[64];
	GetWindowText(GetDlgItem(IDC_PPORTNAME1), comName, 64);
	if (strcmp(comName, "NONE") == 0) {
		dInfo.nPCOMM = 1;
	} else {
		if (dInfo.nPCOMM == 1)
			dInfo.nPCOMM = 2;
		dInfo.pinfo[1].portNum = atoi(&comName[3]) - 1;
	}
	UpdateDriverPortion();
}

void MyDialog::OnInstallDriver(WPARAM wParam, LPARAM lParam)
{
	char comName[64];
	int bAuto;
	if (!(bDriverExist = CheckDriverExist())) {
		AppMessageBox(IDS_NODRIVER, MB_OK | MB_ICONWARNING);
		return;
	}
	GetWindowText(GetDlgItem(IDC_PPORTNAME), comName, 64);
	dInfo.pinfo[1].portNum = atoi(&comName[3]) - 1;
	GetWindowText(GetDlgItem(IDC_PPORTNAME1), comName, 64);
	if (strcmp(comName, "NONE") == 0) {
		dInfo.nPCOMM = 1;
	} else {
		dInfo.pinfo[1].portNum = atoi(&comName[3]) - 1;
		GetWindowText(GetDlgItem(IDC_PPORTNAME2), comName, 64);
		if (strcmp(comName, "NONE") == 0) {
			dInfo.nPCOMM = 2;
		} else {
			dInfo.nPCOMM = 3;
			dInfo.pinfo[2].portNum = atoi(&comName[3]) - 1;
		}
	}

	bAuto = ButtonGetCheck(GetDlgItem(IDC_AUTOSTART));

	if (bDriverInstalled = InstDrv(bAuto)) {
		bDriverInstalled = SetParameters(&dInfo);
	}
	if (bDriverInstalled) {
		bDriverAutoStart = bAuto;
//		AppMessageBox(IDS_DRIVINSTALLED, MB_OK);
	} else {
		AppMessageBox(IDS_DRIVINSTFAILED, MB_OK | MB_ICONERROR);
	}
	UpdateDriverPortion();
}

void MyDialog::OnUninstallDriver(WPARAM wParam, LPARAM lParam)
{
	StopDrv();
	DeinstDrv();

	bDriverRunning = FALSE;
	bDriverInstalled = FALSE;
	bDriverAutoStart = FALSE;

	UpdateDriverPortion();
	UpdateServicePortion();
}

void MyDialog::OnStartDriver(WPARAM wParam, LPARAM lParam)
{
	if (!(bDriverRunning = StartDrv()))
		AppMessageBox(IDS_DRIVSTARTFAILED, MB_OK | MB_ICONERROR);

	UpdateDriverPortion();
	UpdateServicePortion();
}

void MyDialog::OnNpcommClicked(WPARAM wParam, LPARAM lParam)
{
	iCurrentNpcomm = 0;
	UpdateServicePortion();
}

void MyDialog::OnNpcomm1Clicked(WPARAM wParam, LPARAM lParam)
{
	iCurrentNpcomm = 1;
	UpdateServicePortion();
}

void MyDialog::OnNpcomm2Clicked(WPARAM wParam, LPARAM lParam)
{
	iCurrentNpcomm = 2;
	UpdateServicePortion();
}

void MyDialog::OnInstallService(WPARAM wParam, LPARAM lParam)
{
	GetWindowText(GetDlgItem(IDC_EXTRAARGUMENTS), arguments[iCurrentNpcomm], MAX_PATH);

	if (CheckNpcommExist()) {
		if (NTServiceInstall(iCurrentNpcomm, arguments[iCurrentNpcomm])) {
				AppMessageBox(IDS_SERVICEINSTALLED, MB_OK);
			bService[iCurrentNpcomm] = TRUE;
		} else {
			AppMessageBox(IDS_INSTSERVICEFAILED, MB_OK | MB_ICONSTOP);
		}
		UpdateServicePortion();
	} else {
		AppMessageBox(IDS_NONPCOMM, MB_OK | MB_ICONSTOP);
	}
}

void MyDialog::OnUninstallService(WPARAM wParam, LPARAM lParam)
{
	if (NTServiceRemove(iCurrentNpcomm)) {
//		AppMessageBox(IDS_SERVICEDELETED, MB_OK);
		bService[iCurrentNpcomm] = FALSE;
	} else {
		AppMessageBox(IDS_SERVICEDELETEFAILED, MB_OK | MB_ICONSTOP);
	}
	UpdateServicePortion();
}
