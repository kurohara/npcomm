/* V2Mgr.cpp */
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
#include "resource.h"
#include <stdio.h>
#include <process.h>

#include "V2Mgr.h"
#include "UICtrl.h"
#include "pcommdrv.h"
#include "pcommXs.h"
#include "DialogUI.h"

extern char *pTransportPath, *pDevicePath;
extern int gbDebug;
extern int AppMessageBox(unsigned int resid, unsigned int mask);

extern char NpcommDirectory[MAX_PATH];
int V2Manager::numManagers = 0;

V2Manager::V2Manager(HINSTANCE hInstance, int iMgr)
{
	int i;

	mgrIndex = iMgr;
	if (mgrIndex == 0) {
		strcpy(MgrName, APPNAME);
	} else {
		sprintf(MgrName, "%s%d", APPNAME, mgrIndex);
	}
	numManagers++;

	// initialize transport data
	currentTransport = NULL;
	for (i=0;i<MAX_TRANSPORTS;i++) {
		dTransports[i].id = -1;
		dTransports[i].pTransport = NULL;
		dTransports[i].pName = NULL;
		dTransports[i].hModule = NULL;
	}
	//
	version = INTERFACEVERSION;

	this->hInstance = hInstance;
	OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&OSVer);
	hWinApp = NULL;
	pDevice = NULL;
	idCurrentTransport = 0;
	nTransports = 0;
	nextTransportId = 0;
	bOpened = FALSE;
	currentTransport = NULL;

	npVersionNumber = NPVERSIONNUM;
	pathHelpFile[0] = (char) 0;

	InitializeCriticalSection(&dspCrst);
	//
}

V2Manager::~V2Manager()
{
	int i;
	DeleteCriticalSection(&dspCrst);
	for (i=0;i<MAX_TRANSPORTS;i++) {
		if (dTransports[i].pTransport != NULL)
			DeleteTransport(dTransports[i].id);
		if (dTransports[i].pName != NULL)
			free(dTransports[i].pName);
	}

}

// profile functions
int V2Manager::GetProfileString(char *Section, char *Key, char *Value, int Size)
{
	return GetPrivateProfileString(Section, Key, "", Value, Size, IniFilePath);
}

int V2Manager::GetProfileInt(char *Section, char *Key, int DefValue)
{
	return GetPrivateProfileInt(Section, Key, DefValue, IniFilePath);
}

int V2Manager::WriteProfileString(char *Section, char *Key, char *Value)
{
	return WritePrivateProfileString(Section, Key, Value, IniFilePath);
}

int V2Manager::WriteProfileInt(char *Section, char *Key, int Value)
{
	char intStr[256];
	sprintf(intStr, "%d", Value);
	return WritePrivateProfileString(Section, Key, intStr, IniFilePath);
}

// transport administration functions
int V2Manager::RegisterTransport(HINSTANCE hModule, PCommTransport *tr, char *name)
{
	if (nextTransportId < 0)
		return FALSE;
	dTransports[nTransports].pTransport = tr;
	dTransports[nTransports].hModule = hModule;
	dTransports[nTransports].pName = (name != NULL ? strdup(name) : NULL);
	dTransports[nTransports].id = nextTransportId++;
	((MainFrame *)pMainDialog)->AddXPMode(dTransports[nTransports].id, name);
	nTransports++;
	return TRUE;
}

int V2Manager::DeleteTransport(int trId)
{
	int i, j;
	AdmTransport *pCur = dTransports;
	HINSTANCE trHandle = NULL;
	for (i=0;i<nTransports;i++) {
		if (trId == pCur->id) {
			((MainFrame *)pMainDialog)->DeleteXPMode(trId);
			if (pCur->pName != NULL) {
				free(pCur->pName);
				pCur->pName = NULL;
			}
			if (pCur->pTransport != NULL) {
				pCur->pTransport->Release();
				delete pCur->pTransport;
				pCur->pTransport = NULL;
			}
			trHandle = pCur->hModule;
			//
			for (j=i+1;j<MAX_TRANSPORTS;j++) {
				dTransports[j-1] = dTransports[j];
			}
			nTransports--;
			break;
		}
		pCur++;
	}
	if (trHandle == NULL)
		return FALSE;
	int cnt=0;
	pCur = dTransports;
	for (i=0;i<nTransports;i++) {
		if (pCur->id == trId)
			continue;
		if (pCur->hModule == trHandle)
			cnt++;
		pCur++;
	}
	if (cnt == 0) {
		// free asynchronousely this module here.
		// or set trHandle to free queue. 
	}
	return TRUE;
}

int V2Manager::Query(int Command, void *data)
{
	HANDLE NpcommMutex;
	int mState;
	char mutexName[MAX_PATH];
	int i, cnt;
	PSECURITY_DESCRIPTOR    pSD;
	SECURITY_ATTRIBUTES     sa;

	switch (Command) {
	case MQC_NUMAPPS:

		pSD = (PSECURITY_DESCRIPTOR) malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
		InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE);
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = pSD;
		sa.bInheritHandle = TRUE;

		cnt = 1;
		for (i=0;i<3;i++) {
			if (i == mgrIndex)
				continue;
			if (i==0) {
				strcpy(mutexName, "Kurohara_NPCOMM");
			} else {
				sprintf(mutexName, "Kurohara_NPCOMM%d", i);
			}
			NpcommMutex = CreateMutex(&sa, FALSE, mutexName);
			mState = WaitForSingleObject(NpcommMutex, 1);
			if (mState == WAIT_TIMEOUT) {
				cnt++;
			} else {
				ReleaseMutex(NpcommMutex);
			}
			CloseHandle(NpcommMutex);
		}

		free((void *) pSD);
		return cnt;
	default:
		break;
	}
	return FALSE;
}

int V2Manager::Control(int Command, void *data)
{
	UIControl *pUIC;

	switch (Command) {
	case MCC_UI_CTRL:
		pUIC = (UIControl *) data;
		return ((MainFrame *)pMainDialog)->Control(pUIC->Command, pUIC->pData);
		break;
	case MCC_SHUTDOWN:
		if (pDevice != NULL) {
			pDevice->Disconnect();
			pDevice->DeviceRelease();
			delete pDevice;
			pDevice = NULL;
			return TRUE;
		}
		break;
	}
	return FALSE;
}


int V2Manager::TransportSelect(int trId)
{
	int i;
	PCommTransport *pTrans;
	if (trId >= nextTransportId)
		return FALSE;
	if (bOpened)
		return FALSE;
	idCurrentTransport = trId;
	if (!(pTrans = GetTransportById(trId)))
		return FALSE;
	currentTransport = pTrans;
	for (i=0;i<nTransports;i++) {
		int bSelected;
		if (trId == dTransports[i].id) {
			bSelected = TRUE;
			WriteProfileString(SECTION_GENERAL, KEY_TRANSPORTNAME, 
			(dTransports[0].pName == NULL ? "" : dTransports[i].pName));
		} else
			bSelected = FALSE;
		dTransports[i].pTransport->Control(CM_SELECT_TRANSPORT, (void *) bSelected );
	}

	return TRUE;
}

void V2Manager::DoLocalPortChange()
{
	pDevice->ChangeCommName();
}

void V2Manager::OpenHelpPage(int pageId)
{
	WinHelp(hWinApp, pathHelpFile, HELP_CONTEXT, pageId);
}

void V2Manager::ExitManager()
{
	if (bOpened)
		return;
	if (bDoRun == FALSE)
		return;
	bDoRun = FALSE;
	if (WaitForSingleObject(hMainThread, 1500) == WAIT_TIMEOUT) {
		TerminateThread(hMainThread, 0);
	}
	CloseHandle(hMainThread);

	if (pDevice != NULL) {
		Control(MCC_SHUTDOWN, NULL);
	}
	int i;
	for (i=0;i<nTransports;i++) {
		dTransports[i].pTransport->Release();
	}
}

PCommTransport *V2Manager::GetTransportById(int id)
{
	if (id >= nextTransportId)
		return NULL;
	int i;
	for (i=0;i<nTransports;i++) {
		if (id == dTransports[i].id)
			return dTransports[i].pTransport;
	}
	return NULL;
}

//
void V2Manager::Run(void)
{
	int Command;
	int dataSize;
	int ret;

	if (pDevice == NULL)
		return;

	////
	while (1) {
		ret = pDevice->GetPCommCommand(1500, &Command, &dataSize, pcHead);
		if (!bDoRun)
			break;
		if (ret == WAIT_TIMEOUT || ret == WAIT_FAILED)
			continue;
		switch (Command) {
		case XDC_SEND:
		case XDC_CONFIG:
		case XDC_TXCHAR:
		case XDC_PORTCTRL:
			break;
		case XDC_OPEN:
			// to lockout early dispatch
			EnterCriticalSection(&dspCrst);
			//
			bOpened = TRUE;
			((MainFrame *)pMainDialog)->SetTrayIconDefer(IDI_TRAY_ON);
			if (currentTransport != NULL) {
				ret = currentTransport->Dispatch(Command, dataSize, pcHead);
				if (ret) {
					if (!Dispatch(MDC_ACKNOWLEDGE, 0, (void *) MC_ACK_OK)) {
						// may be too late
						bOpened = FALSE;
						((MainFrame *)pMainDialog)->SetTrayIconDefer(IDI_TRAY_NORMAL);
						Command = XDC_CLOSE;
						LeaveCriticalSection(&dspCrst);
						break;
					}
				} else {
					bOpened = FALSE;
					((MainFrame *)pMainDialog)->SetTrayIconDefer(IDI_TRAY_NORMAL);
					Dispatch(MDC_ACKNOWLEDGE, 0, (void *) MC_ACK_ERROR);
				}
			} else {
				bOpened = FALSE;
				((MainFrame *)pMainDialog)->SetTrayIconDefer(IDI_TRAY_NORMAL);
				Dispatch(MDC_ACKNOWLEDGE, 0, (void *) MC_ACK_ERROR);
			}
			LeaveCriticalSection(&dspCrst);
			continue;
		case XDC_CLOSE:
			EnterCriticalSection(&dspCrst);
			bOpened = FALSE;
			((MainFrame *)pMainDialog)->SetTrayIconDefer(IDI_TRAY_NORMAL);
			if (currentTransport != NULL)
				ret = currentTransport->Dispatch(Command, dataSize, pcHead);
			LeaveCriticalSection(&dspCrst);
			continue;
		default:
			break;
		}

		if (currentTransport != NULL)
			ret = currentTransport->Dispatch(Command, dataSize, pcHead);

	}

}

// transport interface functions
int V2Manager::Dispatch(int Command, int DataSize, void *pData)
{
	int rval = FALSE;
	if (pDevice == NULL)
		return FALSE;

	EnterCriticalSection(&dspCrst);

	switch (Command) {
	case MDC_WRITE:
		rval = pDevice->WriteData(DataSize, (char *) pData);
		break;
	case MDC_TXD_NOTIFY:
		rval = pDevice->Notify(PCommDevice::NK_TXDATA, NULL);
		break;
	case MDC_EV_NOTIFY:
		rval = pDevice->Notify(PCommDevice::NK_EVENT, (EventData *) pData);
		break;
	case MDC_ACKNOWLEDGE:
		rval = pDevice->CommandAck(
			((int) pData == MC_ACK_OK ? PCommDevice::PCA_OK : PCommDevice::PCA_ERROR)
			);
		break;
	case MDC_SETCONFIG:
		rval = pDevice->SetCommConfig((CommConfig *) pData);
		break;
	case MDC_SETMODEMSTAT:
		rval = pDevice->SetModemStat((int) pData);
		break;
	case MDC_DISCONNECT:
		rval = pDevice->Disconnect();
		if (bOpened) {
			AppMessageBox(IDS_CONNRESET, MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
		}
		break;
	default:
		break;
	}

	LeaveCriticalSection(&dspCrst);

	return rval;
}

int V2Manager::Initialize()
{
	//
	///////
	HANDLE NpcommMutex;
	int mState;
	char mutexName[MAX_PATH];

	PSECURITY_DESCRIPTOR    pSD;
	SECURITY_ATTRIBUTES     sa;

	pSD = (PSECURITY_DESCRIPTOR) malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = TRUE;

	sprintf(mutexName, "Kurohara_%s", MgrName);
	NpcommMutex = CreateMutex(&sa, FALSE, mutexName);
	mState = WaitForSingleObject(NpcommMutex, 1);
	if (mState == WAIT_TIMEOUT) {
		AppMessageBox(IDS_ALREADYRUNNING, MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	///////
	//
	GetModuleFileName(hInstance, IniFilePath, MAX_PATH);
	char *pDelim = strrchr(IniFilePath, '\\');
	*(++pDelim) = (char) 0;
	strcat(IniFilePath, MgrName);
	strcat(IniFilePath, ".ini");
	//////////////////////////////////////////
	// for ChangeFocus
	bChangeFocus = GetProfileInt(SECTION_GENERAL, KEY_CHANGEFOCUS, 88);
	if (bChangeFocus == 88) {
		bChangeFocus = 0;
		WriteProfileInt(SECTION_GENERAL, KEY_CHANGEFOCUS, bChangeFocus);
	}
	//////////////////////////////////////////
	// for Debug
	gbDebug = GetProfileInt(SECTION_GENERAL, KEY_ISDEBUG, 0);
	return TRUE;
}

int V2Manager::InitializeTransports()
{
	char findPath[MAX_PATH], fileName[MAX_PATH];
	WIN32_FIND_DATA fData;
	HANDLE hFind;
	HINSTANCE hModule;
	int (*init)(PCommManager *pMgr);
//	int (*initSheet)(PCommManager *pMgr);
	PCommTransport *pCurTransport;
	int ret, i;

	nTransports = 0;

//	GetCurrentDirectory(MAX_PATH, findPath);
	strcpy(findPath, NpcommDirectory);
	if (pTransportPath != NULL) {
		strcat(findPath, "\\");
		strcat(findPath, pTransportPath);
	}
	sprintf(fileName, "%s\\*.xpm", findPath);
	hFind = FindFirstFile(fileName, &fData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			sprintf(fileName, "%s\\%s", findPath, fData.cFileName);
			hModule = LoadLibrary(fileName);
			if (hModule == NULL)
				continue;
			init = (int (*)(PCommManager *)) 
					GetProcAddress(hModule, "InitializeTransport");
			if (init != NULL) {
				ret = (*init)(this);
				if (!ret) {
					FreeLibrary(hModule);
					continue;
				}
			} else {
				FreeLibrary(hModule);
				continue;
			}
		} while (FindNextFile(hFind, &fData)) ;

		FindClose(hFind);
	}
	if (nTransports <= 0) {
		currentTransport = NULL;
		return TRUE;
	}

	for (i=0;i<nTransports;i++) {
		pCurTransport = dTransports[i].pTransport;
		// call initialization method if present
		if (pCurTransport) {
			if (!pCurTransport->Initialize(i)) {
				DeleteTransport(i);
			}
		}
	}
	((MainFrame *)pMainDialog)->EnableApplyButton(FALSE);

	char transName[MAX_PATH];
	int lenStr;
	idCurrentTransport = MAX_TRANSPORTS;
	currentTransport = NULL;
	lenStr = GetProfileString(SECTION_GENERAL, KEY_TRANSPORTNAME, transName, MAX_PATH);

	if (lenStr <= 0) {
		currentTransport = dTransports[0].pTransport;
		idCurrentTransport = dTransports[0].id;
	} else {
		for (i=0;i<nTransports;i++) {
			if (strcmp(transName, dTransports[i].pName) == 0) {
				idCurrentTransport = dTransports[i].id;
				currentTransport = dTransports[i].pTransport;
				break;
			}
		}
		if (idCurrentTransport == MAX_TRANSPORTS) {
			currentTransport = dTransports[0].pTransport;
			idCurrentTransport = dTransports[0].id;
			WriteProfileString(SECTION_GENERAL, KEY_TRANSPORTNAME, 
				(dTransports[0].pName == NULL ? "" : dTransports[0].pName));
		}
	}
	if (currentTransport)
		currentTransport->Control(CM_SELECT_TRANSPORT, (void *) TRUE);

	((MainFrame *)pMainDialog)->SetInitialXPMode(idCurrentTransport);

	return TRUE;
}

int V2Manager::InitializeDevice()
{
	char findPath[MAX_PATH], fileName[MAX_PATH];
	WIN32_FIND_DATA fData;
	HANDLE hFind;
	HINSTANCE hModule;
	PCommDevice * (*init)(PCommManager *pMgr);

	strcpy(findPath, NpcommDirectory);
	if (pDevicePath != NULL) {
		strcat(findPath, "\\");
		strcat(findPath, pDevicePath);
	}
	sprintf(fileName, "%s\\*.dll", findPath);
	hFind = FindFirstFile(fileName, &fData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			sprintf(fileName, "%s\\%s", findPath, fData.cFileName);
			hModule = LoadLibrary(fileName);
			if (hModule == NULL)
				continue;
			init = (PCommDevice * (*)(PCommManager *)) 
					GetProcAddress(hModule, "Initialize");
			if (init != NULL) {
				pDevice = (*init)(this);
				if (pDevice != NULL) {
					break;
				}
			}
			FreeLibrary(hModule);
		} while (FindNextFile(hFind, &fData)) ;
		
		FindClose(hFind);
	}
	if (pDevice == NULL)
//		return FALSE;
	{
		pDevice = new tmpDevice;
	}
	if (!pDevice->DeviceInitialize(LocalPortName, mgrIndex)) {
		delete pDevice;
		pDevice = new tmpDevice;
		pDevice->DeviceInitialize(LocalPortName, mgrIndex);
	}
	pcBuf = (char *) malloc(pDevice->maxSendSize + 256);
	pcHead = pcBuf + 256;
	return TRUE;
}

void _stdcall MainManagerThread(V2Manager *pMgr)
{
	if (gbDebug) {
		pMgr->Run();
		_endthreadex(0);
	} else {
		try {
			pMgr->Run();
			_endthreadex(0);
		}
		catch (...) {
			pMgr->Control(MCC_SHUTDOWN, NULL);
		}
	}

}

int V2Manager::StartMainManagerThread(void)
{
	unsigned int thId;
	bDoRun = TRUE;

	hMainThread = (HANDLE) 
		_beginthreadex(
		NULL, 0,
		(unsigned int (_stdcall *)(void *)) MainManagerThread,
		(void *) this, 0, &thId);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
int tmpDevice::GetPCommCommand(int Timeout, int *Command, int *DataSize, void *Data)
{
	static cCount = 0;

	Sleep(Timeout);
#if 0
	if (cCount == 0) {
		cCount = 1;
		*Command = XDC_OPEN;
	} else {
		cCount = 0;
		*Command = XDC_CLOSE;
	}
#else
	*Command = 0;
#endif

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////
