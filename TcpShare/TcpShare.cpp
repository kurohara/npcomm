/***********************************************************************
    TcpShare.cpp
    The Main implementation part of TcpShare.xpm.

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
#include <stdio.h>
#include <process.h>

#include "PCommMgr.h"
#include "TcpShDlg.h"
#include "TcpShare.h"
#include "Sock.h"

// for debug
#include "LogDrv.h"

//
#define DEFAULT_INT	99

HINSTANCE gMyHandle;

BOOL WINAPI DllMain(
    HINSTANCE  hinstDLL,	// handle to DLL module 
    DWORD  fdwReason,	// reason for calling function 
    LPVOID  lpvReserved 	// reserved 
   )
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		gMyHandle = hinstDLL;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

extern "C" DllExport int InitializeTransport(PCommManager *pManager)
{
	TcpShare *pTrans = new TcpShare;

	if (!pTrans)
		return FALSE;
	pTrans->pMgr = pManager;

	return pManager->RegisterTransport(gMyHandle, pTrans, NAME_TRANSPORT);

}

int XpmMessageBox(unsigned int resId, unsigned int mask)
{
	char message[1024];
	if (LoadString(gMyHandle, resId, message, 1023) == 0) return -1;
	return MessageBox(NULL, message, NAME_TRANSPORT, mask);
}

////////////////////////////////////////////////////////////////////////
void SockSideThread(TcpShare *pX)
{
	// this function is entry point of new rx side thread.
	// if you hate creating extra thread, you can call ss.Recv()
	// from window message.
	pX->SockMainLoop();
	_endthread();
}

////////////////////////////////////////////////////////////////////////
static char *CommNameList[NUM_REMOTEPORTS] = PORTNAMELIST;

TcpShare::TcpShare()
{
	int i;
	version = INTERFACEVERSION;
	InitializeCriticalSection(&paramCrst);
	for (i=0;i<MAX_SETTINGS;i++) {
		params.settingItems[i].pAlias = NULL;
		params.settingItems[i].pHostName = NULL;
		params.settingItems[i].pPortName = NULL;
	}
	
	NumPorts = NUM_REMOTEPORTS;
	// change here if support portnames other than "COM?".
	for (i=0;i<NumPorts;i++) {
		PortNameList[i] = CommNameList[i];
	}

	bConnected = FALSE;
	//
	// create downstream object
	pDrive = new ShareSock;

}

TcpShare::~TcpShare()
{
	int i;
	DeleteCriticalSection(&paramCrst);
	for (i=0;i<MAX_SETTINGS;i++) {
		if (params.settingItems[i].pAlias != NULL)
			free(params.settingItems[i].pAlias);
		if (params.settingItems[i].pHostName != NULL)
			free(params.settingItems[i].pHostName);
		if (params.settingItems[i].pPortName != NULL)
			free(params.settingItems[i].pPortName);
	}

	if (pDrive) delete pDrive;
}

int TcpShare::Initialize(int Id)
{
	if (!pDrive)
		return FALSE;

	// insert logger if required
	if (pMgr->GetProfileInt(SECTION_PORTSHARE, KEY_DOLOG, 0)) {
		pDrive = new LogDrv(pDrive, "ShLog.txt");
	}
	//

	pDrive->SetUpStream(this);

	trId = Id;
	/////////////////////////////////////
	LoadSetting();
	/////////////////////////////////////
	// prepare dialog sheet
	pMySheet = NULL;
	if (pMgr->hWinApp) {
		Sheet tmpSheet;

		pMySheet = new TcpShareDlg(pMgr->hWinApp);
		// set reference to me.
		pMySheet->pMyTransport = this;
		pMySheet->Create(gMyHandle);
		tmpSheet.hDlg = pMySheet->hDlg;
		tmpSheet.trId = trId;
		strcpy(tmpSheet.title, NAME_TRANSPORT);
		
		MgrUIControl(UIC_SHEET_ADD, &tmpSheet);
	}
	return TRUE;
}

int TcpShare::Release()
{
	if (pMySheet) {
		MgrUIControl(UIC_SHEET_DEL, pMySheet->hDlg);
		DestroyWindow(pMySheet->hDlg);
		delete pMySheet;
	}
	delete pDrive;
	return TRUE;
}

//
// Manager calls this function with data buffer which have
// prepended temporary area(256 bytes).
// This area is used for decrease memory copy.
int TcpShare::Dispatch(int Command, int dataSize, void *data)
{
	switch (Command) {
	case XDC_SEND:
		pDrive->TxData(dataSize, (char *) data);
		break;
	case XDC_OPEN:
		CacheParameters();
		if (pDrive->Connect(RemoteHostName, IPServicePortNum, (void *) bTcpCallback)) {
			if (pDrive->OpenPort(RemotePortName, &currentConfig, &currentModemStat)) {
				pMgr->Dispatch(MDC_SETCONFIG, sizeof (CommConfig), &currentConfig);
				pMgr->Dispatch(MDC_SETMODEMSTAT, 0, (void *) currentModemStat);
				StartSockSideThread();
				bConnected = TRUE;
				return TRUE;
			} else {
				pDrive->Disconnect();
			}
		}
		return FALSE;
	case XDC_CLOSE:
		pDrive->ClosePort();
		pDrive->Disconnect();
		bConnected = FALSE;
		return TRUE;
	case XDC_CONFIG:
		pDrive->SetConfig((CommConfig *) data);
		currentConfig = *((CommConfig *) data);
		break;
	case XDC_TXCHAR:
		pDrive->TxEMD(*((short *) data));
		break;
	case XDC_PORTCTRL:
		pDrive->PortCtrl(*((int *) data));
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

int TcpShare::Control(int Command, void *data)
{
	switch (Command) {
	case CM_SELECT_TRANSPORT:
		if (pMySheet) pMySheet->Selected((int) data);
		break;
	case CM_RELOAD_SETTING:
		LoadSetting();
		break;
	case CM_APPLYCHANGE:
		if (pMySheet) pMySheet->ApplyChange();
		break;
	case CM_CANCELCHANGE:
		if (pMySheet) pMySheet->CancelChange();
		break;
	case CM_DDE:
		if (pMySheet) pMySheet->DoDDE((XS_DDE *) data);
		break;
	case CM_EXMENU_SELECT:
		if (pMySheet) pMySheet->ExMenuSelected((int) data);
		break;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////

void TcpShare::buildUpSettingList(char *pSettingString)
{
	char *pSettings[MAX_SETTINGS];
	char *pValue;
	int i;

	params.nSettingItems = 0;
	pSettings[0] = strtok(pSettingString, ":");
	if (pSettings[0] == NULL)
		return ;
	params.nSettingItems = 1;
	for (i=1;i<MAX_SETTINGS;i++) {
		pSettings[i] = strtok(NULL, ":");
		if (pSettings[i] == NULL)
			break;

		params.nSettingItems++;
	}

	for (i=0;i<params.nSettingItems;i++) {
		if (params.settingItems[i].pPortName != NULL)
			free(params.settingItems[i].pPortName);
		if (params.settingItems[i].pAlias != NULL)
			free(params.settingItems[i].pAlias);
		if (params.settingItems[i].pHostName != NULL)
			free(params.settingItems[i].pHostName);

		if (*(pSettings[i]) == ',') {
			pValue = strtok(pSettings[i], ",");
			if (pValue == NULL) {
				break;
			}
			params.settingItems[i].pAlias = strdup("");
		}
		else {
			pValue = strtok(pSettings[i], ",");
			if (pValue == NULL) {
				break;
			}
			params.settingItems[i].pAlias = strdup(pValue);
			pValue = strtok(NULL, ",");
		}
		params.settingItems[i].pHostName = strdup(pValue);
		pValue = strtok(NULL, ",");
		if (pValue == NULL) {
			if (i > 0)
				params.nSettingItems = i - 1;
			else
				params.nSettingItems = 0;
			break;
		}
		params.settingItems[i].pPortName = strdup(pValue);
	}
}

void TcpShare::makeListString(char *pString, int maxLen)
{
	int i, lenStr;
	char tmpStr[MAX_PATH], *pOrig = pString;
	*pString = (char) 0;
	for (i=0;i<params.nSettingItems;i++) {
		_snprintf(tmpStr, MAX_PATH, "%s,%s,%s:", 
			params.settingItems[i].pAlias,
			params.settingItems[i].pHostName,
			params.settingItems[i].pPortName);
		lenStr = strlen(tmpStr);
		if ((int) (pString - pOrig) + lenStr > maxLen)
			break;
		strcat(pString, tmpStr);
		pString += lenStr;
	}
}


void TcpShare::CacheParameters()
{
	EnterCriticalSection(&paramCrst);
	bTcpCallback = params.bTcpCallback;
	IPServicePortNum = params.IPServicePortNum;
	strcpy(RemoteHostName, params.RemoteHostName);
	strcpy(RemotePortName, params.RemotePortName);
	LeaveCriticalSection(&paramCrst);
}

void TcpShare::StartSockSideThread()
{
	_beginthread((void (*)(void *))SockSideThread, 0, this);
}

///////////////////////////////////////////////////////////////////
void TcpShare::CancelConnection()
{
	pDrive->Disconnect();

}

void TcpShare::SockMainLoop()
{
	// this function is invoked as new thread.
	// you can call pDrive->Recv() from window message if you love it.
//
	// Previout version(1.5) was doing NULL packet check by this interval.
	// In this version, I disabled it.
	// so current protorol processor may not care about
	// timeout in Recv().
	pDrive->timeout = 15000;

	try {
		while (pDrive->Recv() != SerialDrive::RECV_FAIL) {
		}
		pMgr->Dispatch(MDC_DISCONNECT, 0, NULL);
	}
	catch (...) {
		ShutdownUp();
		throw;
	}
}

/////////////////////////////////////////////////////////////////////
// 
// callback functions
int TcpShare::WriteToUp(int size, char *pData)
{
	int nWritten;
	while (size > 0) {
		nWritten = pMgr->Dispatch(MDC_WRITE, size, pData);
		if (nWritten < 0)
			return FALSE;
		size -= nWritten;
		pData += nWritten;
	}
	return TRUE;
}

int TcpShare::LineBreakNotify()
{
	EventData rEvent;
	rEvent.eventMask = EV_BREAK;
	rEvent.modemStat = currentModemStat;
	rEvent.errorStat = 0;
	return pMgr->Dispatch(MDC_EV_NOTIFY, sizeof (rEvent), &rEvent);
}

int TcpShare::LineErrorNotify(int errStat)
{
	EventData rEvent;
	rEvent.eventMask = EV_ERR;
	rEvent.modemStat = currentModemStat;
	rEvent.errorStat = errStat;
	return pMgr->Dispatch(MDC_EV_NOTIFY, sizeof (rEvent), &rEvent);
}

int TcpShare::EventCharNotify()
{
	EventData rEvent;
	rEvent.eventMask = EV_RXFLAG;
	rEvent.modemStat = currentModemStat;
	rEvent.errorStat = 0;
	return pMgr->Dispatch(MDC_EV_NOTIFY, sizeof (rEvent), &rEvent);
}

int TcpShare::TxEmptyNotify()
{
	EventData rEvent;
	rEvent.eventMask = EV_TXEMPTY;
	rEvent.modemStat = currentModemStat;
	rEvent.errorStat = 0;
	return pMgr->Dispatch(MDC_EV_NOTIFY, sizeof (rEvent), &rEvent);
}

int TcpShare::TxCharNotify()
{
	return pMgr->Dispatch(MDC_TXD_NOTIFY, 0, NULL);
}

int TcpShare::ModemEventNotify(int eventMask, int modemStat)
{
	EventData rEvent;
	rEvent.eventMask = eventMask;
	rEvent.modemStat = modemStat;
	rEvent.errorStat = 0;
	currentModemStat = modemStat;
	return pMgr->Dispatch(MDC_EV_NOTIFY, sizeof (rEvent), &rEvent);
}

int TcpShare::TxFlowOff(int bOff)
{
	// may not called in this release
	return pMgr->Dispatch(MDC_TXFLOW, 0, (void *)bOff);
}

int TcpShare::ShutdownUp()
{
	return pMgr->Control(MCC_SHUTDOWN, NULL);
}

////////////////////////////////////////////////////////
// XsUiIntf part
int TcpShare::ConnectionTest(int bTouch, int *mStat)
{
	int rval = FALSE;
	*mStat = 0;
	CacheParameters();
	if (pDrive->Connect(RemoteHostName, IPServicePortNum, (void *) bTcpCallback)) {
		if (bTouch) {
			if (pDrive->OpenPort(RemotePortName, &currentConfig, &currentModemStat)) {
				*mStat = currentModemStat;
				rval = TRUE;
				pDrive->ClosePort();
			} else {
				*mStat = TRUE;// means only tcp connection succeeded
				rval = FALSE;
			}
		} else
			rval = TRUE;
		pDrive->Disconnect();
	}
	return rval;
}

int TcpShare::getNumPortNames()
{
	return NumPorts;
}

char **TcpShare::getPortNameList()
{
	return PortNameList;
}

void TcpShare::PutParameters(TcpShareParams *newParams)
{
	EnterCriticalSection(&paramCrst);
	params = *newParams;
	LeaveCriticalSection(&paramCrst);
}

void TcpShare::GetParameters(TcpShareParams *newParams)
{
	EnterCriticalSection(&paramCrst);
	*newParams = params;
	LeaveCriticalSection(&paramCrst);
}

int TcpShare::isConnected()
{
	return bConnected;
}

int TcpShare::getTransportId()
{
	return trId;
}

int TcpShare::MgrUIControl(int Command, void *pData)
{
	UIControl uic;

	uic.Command = Command;
	uic.trId = trId;
	uic.pData = pData;

	return pMgr->Control(MCC_UI_CTRL, &uic);

}

#define LOAD_STRING(section, key, pVar, def) \
	(pMgr->GetProfileString(section, key, pVar, MAX_PATH) == 0 ? \
		(strcpy(pVar, def), \
		pMgr->WriteProfileString(section, key, pVar)) : TRUE) 

#define LOAD_INT(section, key, var, nv_def, def) \
	(var = pMgr->GetProfileInt(section, key, nv_def), \
		(var == nv_def ? \
			(var = def, pMgr->WriteProfileInt(section, key, var)) : TRUE)\
	)

#define SAVE_STRING(section, key, pVar) \
		pMgr->WriteProfileString(section, key, pVar)

#define SAVE_INT(section, key, var) \
		pMgr->WriteProfileInt(section, key, var)

void TcpShare::LoadSetting()
{
	int lenStr;
	EnterCriticalSection(&paramCrst);

	// Ip portnumber
	LOAD_INT(SECTION_PORTSHARE, KEY_IPPORTNUM, params.IPServicePortNum, 0, DEF_SERVICEPORTNUMBER);
	// tcp callback flag
	LOAD_INT(SECTION_PORTSHARE, KEY_TCPCALLBACK, params.bTcpCallback, DEFAULT_INT, 1);
	// remote host name
	LOAD_STRING(SECTION_PORTSHARE, KEY_REMOTEHOST, params.RemoteHostName, "");
	// alternate host names
	int i;
	char tmpKey[MAX_PATH];
	for (i=0;i<NUM_HOSTNAMES;i++) {
		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_REMOTEHOST, i);
		LOAD_STRING(SECTION_PORTSHARE, tmpKey, params.RemoteHostNames[i], "");
	}
	//
	// this part will be changed if I need to support serial port name
	/// other than "COM?".
	// remote port name index
	int iIndex;
	LOAD_INT(SECTION_PORTSHARE, KEY_REMOTEPORTIND, iIndex, DEFAULT_INT, 0);
	strcpy(params.RemotePortName, PortNameList[iIndex]);
	
	// auto check
	LOAD_INT(SECTION_PORTSHARE, KEY_AUTOCHECK, params.bAutoCheck, DEFAULT_INT, 1);
	// setting list
	char listString[1024];
	lenStr = pMgr->GetProfileString(SECTION_PORTSHARE, KEY_SETTINGLIST, listString, 1023);
	if (lenStr > 0) {
		buildUpSettingList(listString);
	}
	//
	LeaveCriticalSection(&paramCrst);

}

void TcpShare::SaveSetting()
{
	// Ip portnumber
	pMgr->WriteProfileInt(SECTION_PORTSHARE, KEY_IPPORTNUM, params.IPServicePortNum);
	// tcp callback flag
	pMgr->WriteProfileInt(SECTION_PORTSHARE, KEY_TCPCALLBACK, params.bTcpCallback);
	// remote host name
	pMgr->WriteProfileString(SECTION_PORTSHARE, KEY_REMOTEHOST, params.RemoteHostName);
	// alternate host names
	int i;
	char tmpKey[MAX_PATH];
	for (i=0;i<NUM_HOSTNAMES;i++) {
		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_REMOTEHOST, i);
		pMgr->WriteProfileString(SECTION_PORTSHARE, tmpKey, params.RemoteHostNames[i]);
	}
	// remote port name index
	// this part will be changed if I need to support serial port name
	/// other than "COM?".
	int iIndex = 0;
	for (i=0;i<NumPorts;i++) {
		if (strcmp(params.RemotePortName, PortNameList[i]) == 0) {
			iIndex = i;
			break;
		}
	}
	pMgr->WriteProfileInt(SECTION_PORTSHARE, KEY_REMOTEPORTIND, iIndex);
	//

	// startup pinger
	pMgr->WriteProfileInt(SECTION_PORTSHARE, KEY_AUTOCHECK, params.bAutoCheck);
	// setting list
	char listString[1024];
	makeListString(listString, 1023);
	pMgr->WriteProfileString(SECTION_PORTSHARE, KEY_SETTINGLIST, listString);
	//

}
