/* main.cpp */
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
#include <windows.h>
#include <process.h>
#include "resource.h"

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#define APPNAME "NPCOMM"

#include "V2Mgr.h"
#include "DialogUI.h"

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
int MainMessageLoop(HWND mainWnd);
void NTServiceMain();
void GetUserStation(int bGet);

MainFrame *gpMainDialog = NULL;
V2Manager *gpMgr = NULL;
HINSTANCE ghModule;

char *pTransportPath, *pDevicePath;
char NpcommDirectory[MAX_PATH];
int gbDebug = FALSE;
int gIndexMgr = 0;
int gbIsService = FALSE;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{


	pTransportPath = NULL;
	pDevicePath = NULL;
	char *pArgv = strtok(lpCmdLine, " ");
	while (pArgv != NULL) {
		if (*pArgv == '-') {
			switch (pArgv[1]) {
			case 't':
				pTransportPath = strtok(NULL, " ");
				break;
			case 'd':
				pDevicePath = strtok(NULL, " ");
				break;
			case 's':
				if (stricmp(pArgv, "-service") == 0)
					gbIsService = TRUE;
				break;
			case 'i':
				gIndexMgr = atoi(&pArgv[2]);
				break;
			}
		}
		pArgv = strtok(NULL, " ");
	}

	if (gbIsService) {
		ghModule = hInstance;
		NTServiceMain();
		return TRUE;
	}

	if (!InitApplication(hInstance)) {
		return (FALSE);
	}

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return (FALSE);
	}

	return MainMessageLoop(gpMainDialog->hDlg);
}

int MainMessageLoop(HWND mainWnd)
{
	HANDLE hAccelTable;
	MSG msg;
	hAccelTable = LoadAccelerators (GetModuleHandle(NULL), APPNAME);

	if (gbDebug) {
		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) {

			if (!TranslateAccelerator (msg.hwnd, (HACCEL) hAccelTable, &msg)) {
				if (!IsDialogMessage(gpMainDialog->hDlg, &msg) ) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

	} else {
		try {
			// Main message loop:
			while (GetMessage(&msg, NULL, 0, 0)) {
				if (!TranslateAccelerator (msg.hwnd, (HACCEL) hAccelTable, &msg)) {
					if (!IsDialogMessage(gpMainDialog->hDlg, &msg) ) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				//
			}
		}
		catch (...) {
			if (gpMgr != NULL)
				gpMgr->ExitManager();
			throw;
		}
	}

	if (gpMgr != NULL)
		gpMgr->ExitManager();

	return (msg.wParam);
}

BOOL InitApplication(HINSTANCE hInstance)
{
//    WNDCLASS  wc;
//    HWND      hwnd;

	return TRUE;
}

#include <COMMCTRL.h>
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	GetModuleFileName(hInstance, NpcommDirectory, MAX_PATH);
	char *pDelim = strrchr(NpcommDirectory, '\\');
	*pDelim = (char) 0;

	InitCommonControls();
	gpMgr = new V2Manager(hInstance, gIndexMgr);
	if (!gpMgr->Initialize())
		return FALSE;
	gpMgr->InitializeDevice();
	gpMainDialog = new MainFrame;
	// set cross reference
	gpMgr->pMainDialog = gpMainDialog;
	gpMainDialog->pMgr = gpMgr;
	gpMainDialog->Create(hInstance);

	gpMgr->hWinApp = gpMainDialog->hDlg;
	gpMgr->InitializeTransports();
	gpMgr->StartMainManagerThread();
	return TRUE;
}

VOID SvcDebugOut(LPSTR String, DWORD Status);

int AppMessageBox(unsigned int resid, unsigned int mask)
{
	char message[1024];
	if (LoadString(gpMgr->hInstance, resid, message, 1023) == 0) 
		strcpy(message, "no message");
	if (gbIsService) {
#ifdef MB_SERVICE_NOTIFICATION
		return MessageBox(NULL, message, APPNAME, mask | MB_SERVICE_NOTIFICATION);
#endif /* MB_SERVICE_NOTIFICATION */
	} else {
		return MessageBox(NULL, message, APPNAME, mask);
	}
	return IDOK;
}

///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Windows NT service portion

SERVICE_STATUS          ServiceStatus;
SERVICE_STATUS_HANDLE   ServiceStatusHandle;

VOID  ServiceStart (DWORD argc, LPTSTR *argv);
VOID  ServiceCtrlHandler (DWORD opcode);
void MainServiceThread(HANDLE hEvent);

SERVICE_TABLE_ENTRY   gDispatchTable[] =
    {
        {TEXT("NPCOMM"), (LPSERVICE_MAIN_FUNCTIONA) ServiceStart}, 
        {NULL, NULL}
    };
SERVICE_TABLE_ENTRY   gDispatchTable1[] =
    {
        {TEXT("NPCOMM1"), (LPSERVICE_MAIN_FUNCTIONA) ServiceStart}, 
        {NULL, NULL}
    };
SERVICE_TABLE_ENTRY   gDispatchTable2[] =
    {
        {TEXT("NPCOMM2"), (LPSERVICE_MAIN_FUNCTIONA) ServiceStart}, 
        {NULL, NULL}
    };

VOID SvcDebugOut(LPSTR String, DWORD Status)
{
    CHAR  Buffer[BUFSIZ];
    if (strlen(String) < BUFSIZ) 
    { 
        sprintf(Buffer, String, Status); 
        OutputDebugStringA(Buffer); 
    } 
}

void NTServiceMain()
{
	SERVICE_TABLE_ENTRY *pTable;
	if (gIndexMgr == 0)
		pTable = gDispatchTable;
	else
	if (gIndexMgr == 1)
		pTable = gDispatchTable1;
	else
	if (gIndexMgr == 2)
		pTable = gDispatchTable2;
	else
		return;
	if (!StartServiceCtrlDispatcher(pTable))
	{
		SvcDebugOut(
			" [NPCOMM] StartServiceCtrlDispatcher error = %d\n",
			GetLastError()); 
	}
}

void ServiceStart (DWORD argc, LPTSTR *argv)
{
	int ret;

	ServiceStatus.dwServiceType        = SERVICE_WIN32;
	ServiceStatus.dwCurrentState       = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP/* |
	/*SERVICE_ACCEPT_PAUSE_CONTINUE */;
	ServiceStatus.dwWin32ExitCode      = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint         = 0;
	ServiceStatus.dwWaitHint           = 0;

	ServiceStatusHandle = RegisterServiceCtrlHandler(
//		TEXT("NPCOMM"),
		argv[0],
		(LPHANDLER_FUNCTION) ServiceCtrlHandler);

    if (ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		SvcDebugOut("[NPCOMM] RegisterServiceCtrlHandler failed %d\n",
			GetLastError());
        return;
	}

	ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
	ServiceStatus.dwCheckPoint         = 0;
	ServiceStatus.dwWaitHint           = 0;
	ServiceStatus.dwWin32ExitCode      = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;

	HANDLE hInitialize = CreateEvent(NULL, TRUE, FALSE, NULL);

	_beginthread((void (*)(void *))MainServiceThread,
		0, (void *) hInitialize);

	ret = WaitForSingleObject(hInitialize, 5000);
//	if (ret == WAIT_TIMEOUT)
//		OutputDebugStringA("Mainthread time out\n"); 

	SetServiceStatus (ServiceStatusHandle, &ServiceStatus);

    return;
}

void MainServiceThread(HANDLE hEvent)
{
	HINSTANCE hModule;
	hModule = GetModuleHandle(NULL);
	//

	if (hEvent == NULL)
		return;
	if (!InitApplication(hModule)) {
		SetEvent(hEvent);
		return;
	}

//        OutputDebugStringA("MainServiceThread()\n"); 
	if (!InitInstance(hModule, 0)) {
//        OutputDebugStringA("MainServiceThread(1)\n"); 
		SetEvent(hEvent);
		return;
	}
//        OutputDebugStringA("MainServiceThread(2)\n"); 

	ServiceStatus.dwCurrentState       = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint         = 0;
	ServiceStatus.dwWaitHint           = 0;

	SetEvent(hEvent);

	MainMessageLoop(gpMainDialog->hDlg);

	ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
	ServiceStatus.dwCheckPoint         = 0;
	ServiceStatus.dwWaitHint           = 0;
	ServiceStatus.dwWin32ExitCode      = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;

	SetServiceStatus (ServiceStatusHandle, &ServiceStatus);
}

void ServiceStop()
{
	gpMgr->ExitManager();
	PostMessage(gpMainDialog->hDlg, WM_QUIT, 0, 0);
	delete gpMainDialog;
}

VOID ServiceCtrlHandler (DWORD Opcode)
{
    DWORD status;

	switch(Opcode)
	{
		case SERVICE_CONTROL_PAUSE:
		// Do whatever it takes to pause here.
			ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			break;
		case SERVICE_CONTROL_CONTINUE:
		// Do whatever it takes to continue here.
			ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			break;
		case SERVICE_CONTROL_STOP:
		// Do whatever it takes to stop here.
		// stop PortServ service
			ServiceStop();
			//
			ServiceStatus.dwWin32ExitCode = 0;
			//MyServiceStatus.dwCurrentState  = SERVICE_STOP_PENDING;
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
			ServiceStatus.dwCheckPoint    = 0;
			ServiceStatus.dwWaitHint      = 0;

			if (!SetServiceStatus (ServiceStatusHandle, &ServiceStatus))
			{
				status = GetLastError();
				SvcDebugOut("[NPCOMM] SetServiceStatus error %ld\n",
				status);
			}
			SvcDebugOut("[NPCOMM] Leaving MyService \n",0);
			return;
		case SERVICE_CONTROL_INTERROGATE:
		// Fall through to send current status.
			break;
		default:
			break;
	}

	// Send current status.
	if (!SetServiceStatus (ServiceStatusHandle,  &ServiceStatus))
	{
		status = GetLastError();
		SvcDebugOut("[NPCOMM] SetServiceStatus error %ld\n",
			status);
	}
	return;
}

void GetUserStation(int bGet)
{
	static HWINSTA hwinstaSave;
	static HDESK hdeskSave;
	static HWINSTA hwinstaUser;
	static HDESK hdeskUser;
	int dwThreadId;

	if (!gbIsService)
		return;

	if (bGet) {
		GetDesktopWindow(); 
		hwinstaSave = GetProcessWindowStation(); 
		dwThreadId = GetCurrentThreadId(); 
		hdeskSave = GetThreadDesktop(dwThreadId); 

		hwinstaUser = OpenWindowStation("WinSta0", FALSE, MAXIMUM_ALLOWED);
		if (hwinstaUser == NULL) {
			return;
		}
		SetProcessWindowStation(hwinstaUser); 
		hdeskUser = OpenDesktop("Default", 0, FALSE, MAXIMUM_ALLOWED); 
		if (hdeskUser == NULL) {
			SetProcessWindowStation(hwinstaSave);
			CloseWindowStation(hwinstaUser);
			return ;
		}
		SetThreadDesktop(hdeskUser);
	} else {
		/* 
		 * Restore window station and desktop. 
		 */ 
		SetThreadDesktop(hdeskSave); 
		SetProcessWindowStation(hwinstaSave); 
		CloseDesktop(hdeskUser); 
		CloseWindowStation(hwinstaUser); 
	}
}
