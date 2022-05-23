/***********************************************************************
    Telnet.cpp
    The main implementation part of Telnet.xpm.

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

#include <Windows.h>
#include "PCommMgr.h"
#include "Telnet.h"
#include "TelnetDlg.h"

#include <stdio.h>

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
	Telnet *pTrans = new Telnet;

	if (!pTrans)
		return FALSE;

	pTrans->pMgr = pManager;

	if (!pManager->RegisterTransport(gMyHandle, pTrans, NAME_TRANSPORT)) {
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////
// Telnet transport class implementations
Telnet::Telnet()
{

	InitializeCriticalSection(&paramCrst);
	version = INTERFACEVERSION;
}

Telnet::~Telnet()
{
	DeleteCriticalSection(&paramCrst);

}

int Telnet::Initialize(int tid)
{
	trId = tid;
	STModule *pMod;

	pStSock = new SockStream;
	if (pStSock == NULL)
		return FALSE;
	// last pushed module is top most one
	pStSock->push(new TelnetProtocol);
	pStSock->push(new ProxyProcess);
	pStSock->push(new VirtualModem);
	//
	pStSock->pMyCallback = (STCallback *) this;
	pStSock->timeToWait = 50000; // set timeout value to 50 sec
	/////////////////////////////////////
	LoadSetting();
	/////////////////////////////////////
	pMySheet = NULL;
	if (pMgr->hWinApp != NULL) {
	// prepare dialog sheet
		Sheet tmpSheet;

		pMySheet = new TelnetDlg(pMgr->hWinApp);
		pMySheet->pMyTransport = this;
		pMySheet->Create(gMyHandle);
		tmpSheet.hDlg = pMySheet->hDlg;
		tmpSheet.trId = trId;
		strncpy(tmpSheet.title, NAME_TRANSPORT, MAX_PATH);
		
		MgrUIControl(UIC_SHEET_ADD, &tmpSheet);
	}
	return TRUE;
}

int Telnet::Release()
{
	delete (pStSock->pop());
	delete (pStSock->pop());
	delete (pStSock->pop());
	delete pStSock;

	if (pMySheet) {
		MgrUIControl(UIC_SHEET_DEL, (void *) pMySheet->hDlg);
		DestroyWindow(pMySheet->hDlg);
		delete pMySheet;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////

void Telnet::CacheParameters()
{
	EnterCriticalSection(&paramCrst);
	////////////////////////////////////////////
	STMessage aMes;
	aMes.type.anyMessage = ProxyProcess::STMESSAGE_ENABLE;
	aMes.pData = (void *) params.bUseProxy;
	pStSock->Control(&aMes);
	aMes.type.anyMessage = ProxyProcess::STMESSAGE_SETPARAMS;
	aMes.pData = & params.proxParams;
	pStSock->Control(&aMes);

	aMes.type.anyMessage = VirtualModem::STMESSAGE_ENABLE;
	aMes.pData = (void *) TRUE;
	pStSock->Control(&aMes);
	aMes.type.anyMessage = VirtualModem::STMESSAGE_CLEARPARAMS;
	pStSock->Control(&aMes);
	if (params.bConnectOnDial) {
		int i;
		for (i=0;i<MAX_SCANNEE;i++) {
			aMes.type.anyMessage = VirtualModem::STMESSAGE_ADDPARAM;
			aMes.pData = (void *) & params.dialParams[i];
			pStSock->Control(&aMes);
		}
	}
	// se ras params
	// rasParams is driver parameters so I don't use control message
	// 
	pStSock->SetRasParams(&params.rasParams);
	////////////////////////////////////////////
	LeaveCriticalSection(&paramCrst);
}

//
// Manager calls this function with data buffer which have
// prepended temporary area(256 bytes).
// This area is used for decrease memory copy.
// ( in this Telnet transport module, this temporary area is not used.)
int Telnet::Dispatch(int Command, int dataSize, void *data)
{

	switch (Command) {
	case XDC_SEND:
		NotifyWritten();
		pStSock->WriteData(dataSize, (char *) data);
		break;
	case XDC_OPEN: // Timeout expires in about 10 seconds.
		CacheParameters();
		if (pStSock->Open()) {
			if (pStSock->Connect(NULL, DEF_TELNETPORT) ) {
				return TRUE;
			}
		}
		return FALSE;
	case XDC_CLOSE:
		pStSock->Close();
		return TRUE;
	case XDC_CONFIG:
		// CommConfig *pConfig = (CommConfig *) data;
		break;
	case XDC_TXCHAR:
		// char c = *((char *) data);
		break;
	case XDC_PORTCTRL:
		// int control = (int) data;
		// control value is same as EscapeCommFunction() argument.
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

int Telnet::Control(int Command, void *data)
{
	switch (Command) {
	case CM_SELECT_TRANSPORT:
		// int bEnabled = (int) data;
		// bEnabled is true if this transport is selected
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
		return FALSE;
	case CM_EXMENU_SELECT:
		if (pMySheet) pMySheet->ExMenuSelected((int) data);
		break;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////
void Telnet::NotifyWritten()
{
	pMgr->Dispatch(MDC_TXD_NOTIFY, 0, NULL);
}

int Telnet::WriteToDevice(int size, char *pData)
{
	return pMgr->Dispatch(MDC_WRITE, size, pData);
}

void Telnet::SetModemState(int modemStat)
{
	EventData rEvent;
	if (modemStat ^ currentModemStat) {
		rEvent.eventMask = 0;
		rEvent.eventMask |= 
			((modemStat & MS_CTS_ON) ^ (currentModemStat & MS_CTS_ON) ? EV_CTS : 0);
		rEvent.eventMask |= ((modemStat & MS_DSR_ON) ^ (currentModemStat & MS_DSR_ON) ? EV_DSR : 0);
		rEvent.eventMask |= ((modemStat & MS_RING_ON) ^ (currentModemStat & MS_RING_ON) ? EV_RING : 0);
		rEvent.eventMask |= ((modemStat & MS_RLSD_ON) ^ (currentModemStat & MS_RLSD_ON) ? EV_RLSD : 0);
		rEvent.modemStat = modemStat;
		currentModemStat = modemStat;
		pMgr->Dispatch(MDC_EV_NOTIFY, sizeof (rEvent), &rEvent);
	}
}

void Telnet::NotifyLineBreak()
{
	EventData rEvent;
	rEvent.eventMask = EV_BREAK;
	pMgr->Dispatch(MDC_EV_NOTIFY, sizeof (rEvent), &rEvent);
}

void Telnet::NotifyDisconnect()
{
	pMgr->Dispatch(MDC_DISCONNECT, 0, NULL);
}

// STCallback implementations
int Telnet::ReadDataCB(int size, char *pData)
{
	return pMgr->Dispatch(MDC_WRITE, size, pData);
}

int Telnet::MessageCB(STMessage *pMes)
{
	switch (pMes->type.anyMessage) {
	case STMESSAGE_DISCONNECTED:
		break;
	case STMESSAGE_SHUTDOWN:
		pMgr->Control(MCC_SHUTDOWN, NULL);
		break;
	default:
		break;
	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////
// XsUiIntf part
void Telnet::PutParameters(TelnetParams *newParams)
{
	EnterCriticalSection(&paramCrst);
	params = *newParams;
	LeaveCriticalSection(&paramCrst);
}

void Telnet::GetParameters(TelnetParams *newParams)
{
	EnterCriticalSection(&paramCrst);
	*newParams = params;
	LeaveCriticalSection(&paramCrst);
}

int Telnet::MgrUIControl(int Command, void *pData)
{
	UIControl uic;

	uic.Command = Command;
	uic.trId = trId;
	uic.pData = pData;

	return pMgr->Control(MCC_UI_CTRL, &uic);

}

int Telnet::getTransportId()
{
	return trId;
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

void Telnet::LoadSetting()
{
	EnterCriticalSection(&paramCrst);

	// proxy flag
	LOAD_INT(SECTION_TELNET, KEY_USEPROXY, params.bUseProxy, DEFAULT_INT, 0);
	// proxy port number
	LOAD_INT(SECTION_TELNET, KEY_PROXYPORT, params.proxParams.ProxyPortNum, 0, DEF_TELNETPORT);
	// proxy server name
	LOAD_STRING(SECTION_TELNET, KEY_PROXYSERVER, params.proxParams.ProxyServerName, "");
	// proxy result string
	LOAD_STRING(SECTION_TELNET, KEY_PROXYRESULT, params.proxParams.ProxyResultString, DEF_PROXYRESULT);
	// proxy time out
	LOAD_INT(SECTION_TELNET, KEY_PROXYTIMEOUT, params.proxParams.ProxyWaitTime, 0, DEF_PROXYTIMEOUT);
	// dial parameters
	LOAD_INT(SECTION_TELNET, KEY_CONNECTONDIAL, params.bConnectOnDial, DEFAULT_INT, 0);
	int i;
	char tmpKey[MAX_PATH];
	for (i=0;i<MAX_SCANNEE;i++) {
		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_DIALCOMMAND, i+1);
		LOAD_STRING(SECTION_TELNET, tmpKey, params.dialParams[i].DialCommand, "");

		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_DESTINATION, i+1);
		LOAD_STRING(SECTION_TELNET, tmpKey, params.dialParams[i].Destination, "");

		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_SERVERPORT, i+1);
		LOAD_INT(SECTION_TELNET, tmpKey, params.dialParams[i].IPPort, 0, DEF_TELNETPORT);
	}
	//
	// kick ras flag
	LOAD_INT(SECTION_TELNET, KEY_KICKRAS, params.rasParams.bKickRas, DEFAULT_INT, 0);
	// ras entry to dial
	LOAD_STRING(SECTION_TELNET, KEY_RASENTRY, params.rasParams.RasEntryName, "");
	//
	LeaveCriticalSection(&paramCrst);

}

void Telnet::SaveSetting()
{
	// proxy flag
	SAVE_INT(SECTION_TELNET, KEY_USEPROXY, params.bUseProxy);
	// proxy port number
	SAVE_INT(SECTION_TELNET, KEY_PROXYPORT, params.proxParams.ProxyPortNum);
	// proxy server name
	SAVE_STRING(SECTION_TELNET, KEY_PROXYSERVER, params.proxParams.ProxyServerName);
	// proxy result string
	SAVE_STRING(SECTION_TELNET, KEY_PROXYRESULT, params.proxParams.ProxyResultString);
	// proxy time out
	SAVE_INT(SECTION_TELNET, KEY_PROXYTIMEOUT, params.proxParams.ProxyWaitTime);
	// dial parameters
	SAVE_INT(SECTION_TELNET, KEY_CONNECTONDIAL, params.bConnectOnDial);
	int i;
	char tmpKey[MAX_PATH];
	for (i=0;i<MAX_SCANNEE;i++) {
		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_DIALCOMMAND, i+1);
		SAVE_STRING(SECTION_TELNET, tmpKey, params.dialParams[i].DialCommand);

		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_DESTINATION, i+1);
		SAVE_STRING(SECTION_TELNET, tmpKey, params.dialParams[i].Destination);

		_snprintf(tmpKey, MAX_PATH, "%s%d", KEY_SERVERPORT, i+1);
		SAVE_INT(SECTION_TELNET, tmpKey, params.dialParams[i].IPPort);
	}
	//
	// kick ras flag
	SAVE_INT(SECTION_TELNET, KEY_KICKRAS, params.rasParams.bKickRas);
	// ras entry to dial
	SAVE_STRING(SECTION_TELNET, KEY_RASENTRY, params.rasParams.RasEntryName);
	//

}
