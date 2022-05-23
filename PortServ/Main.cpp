/***********************************************************************
    Main.cpp
	Main entry point for PortShare server program.

    Copyright (C) 1999-2003 by Hiroyoshi Kurohara
        All Rights Reserved

    This file is part of PortShare server program for Windows.

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
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "TcpMgr.h"
#include "PSDialog.h"
#include "Main.h"
#include <process.h>

#include <stdarg.h>
void SvcDebugPrintf(char *pFormat, ... );
int AppMessageBox(unsigned int resid, unsigned int mask);
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
int MainMessageLoop(HWND mainWnd);
void NTServiceMain();
void NTServiceInstall();
void NTServiceRemove();
int ServiceInstall();
void ServiceRemove();
void KillPSS95(char *pathService);

PortServDlg *gpPSDialog;
OSVERSIONINFO gOsVersion;
int gbIsService = FALSE, gbIsNT = FALSE;
int gbIsInstServ = FALSE, gbIsRemoveServ = FALSE;
char gCurrentDirectory[MAX_PATH];
int _getProfileInt(const char *pSection, const char *pKey, int defVal);
void ParseArgument(char *lpCmdLine);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	ParseArgument(lpCmdLine);

	gOsVersion.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&gOsVersion);
	if (gOsVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		gbIsNT = TRUE;

	if (gbIsNT) {
		if (gbIsService) {
			NTServiceMain();
			return TRUE;
		}
		if (gbIsInstServ) {
			NTServiceInstall();
			return TRUE;
		} else
		if (gbIsRemoveServ) {
			NTServiceRemove();
			return TRUE;
		}
	} else {
		if (gbIsInstServ) {
			if (!ServiceInstall())
				return FALSE;
			// continue working as service
			gbIsService = TRUE;
		} else
		if (gbIsRemoveServ) {
			ServiceRemove();
			return TRUE;
		}
	}

	if (!InitApplication(hInstance)) {
		return FALSE;
	}

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	return MainMessageLoop(gpPSDialog->hDlg);
}

int MainMessageLoop(HWND mainWnd)
{
	MSG msg;
	HANDLE hAccelTable;
	hAccelTable = LoadAccelerators (GetModuleHandle(NULL), APPNAME);

	//
	//
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator (msg.hwnd, (HACCEL) hAccelTable, &msg)) {
			if (!IsDialogMessage(mainWnd, &msg) ) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	// stop TCP service manager
	TcpManager::StopManager();
	return (msg.wParam);
}

extern int InitializeSockSystem();

BOOL InitApplication(HINSTANCE hInstance)
{
	static HANDLE gPortServMutex;
	int mState;

	PSECURITY_DESCRIPTOR    pSD;
	SECURITY_ATTRIBUTES     sa;

	pSD = (PSECURITY_DESCRIPTOR) malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = TRUE;

	gPortServMutex = CreateMutex(&sa, FALSE, "Kurohara_PortServ");
	mState = WaitForSingleObject(gPortServMutex, 1);
	if (mState == WAIT_TIMEOUT) {
		if (!gbIsService)
			AppMessageBox(IDS_ALREADYRUNNING, MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	InitializeSockSystem();

	return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	GetModuleFileName(hInstance, 
						gCurrentDirectory,
						MAX_PATH);
	char *pDelim = 
		strrchr(gCurrentDirectory, '\\');
	*pDelim = (char) 0;

	gpPSDialog = new PortServDlg;
	if (gOsVersion.dwMajorVersion >= 4)
		gpPSDialog->bHasSysTray = TRUE;
	else
		gpPSDialog->bHasSysTray = FALSE;

	gpPSDialog->Create(hInstance);

	int servicePort =
	_getProfileInt("TCPIP", "IPPortNumber", DEF_SERVICEPORT);
	TcpManager::StartManager(servicePort);

	return TRUE;
}

//////////////////////////////////////////////////////////////////
int AppMessageBox(unsigned int resid, unsigned int mask)
{
	char message[1024];
	if (LoadString(GetModuleHandle(NULL), resid, message, 1023) == 0) 
		return -1;
	return MessageBox(NULL, message, APPNAME, mask);
}

int _getProfileString(const char *pSection, const char *pKey, char *pString, int maxlen)
{
	char iniFilePath[MAX_PATH];
	sprintf(iniFilePath, "%s\\%s.ini",
				gCurrentDirectory, APPNAME);

	return GetPrivateProfileString(pSection, pKey,
			"", pString, maxlen, iniFilePath);
}

int _putProfileString(const char *pSection, const char *pKey, char *pVal)
{
	char iniFilePath[MAX_PATH];
	sprintf(iniFilePath, "%s\\%s.ini",
				gCurrentDirectory, APPNAME);

	return WritePrivateProfileString(pSection, pKey, pVal, iniFilePath);
}

int _getProfileInt(const char *pSection, const char *pKey, int defVal)
{
	char intString[32];
	int iVal;

	if (_getProfileString(pSection, pKey, intString, 32) > 0) {
		iVal = atoi(intString);
	} else {
		iVal = defVal;
		// no value, write out now.
		sprintf(intString, "%d", iVal);
		_putProfileString(pSection, pKey, intString);
	}

	return iVal;
}

void ParseArgument(char *pArgString)
{
	int i;
	char *argv[10];// up to 10 argument strings
	int argc = 0;
	argv[0] = strtok(pArgString, " ");
	if (argv[0] == 0)
		return;
	argc = 1;
	for (i=1;i<10;i++) {
		argv[i] = strtok(NULL, " ");
		if (argv[i] == NULL)
			break;
		argc++;
	}

	for (i=0;i<argc;i++) {
		if (stricmp(argv[i], "-service") == 0) {
			gbIsService = TRUE;
			return;
		}
		else
		if (stricmp(argv[i], "-instserv") == 0) {
			gbIsInstServ = TRUE;
			return;
		}
		else
		if (stricmp(argv[i], "-deleteserv") == 0) {
			gbIsRemoveServ = TRUE;
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////
// Windows NT service portion
void SvcDebugPrintf(char *pFormat, ... )
{
	char outBuf[1024];
	va_list args;
	va_start(args, pFormat);

	_vsnprintf(outBuf, 1024, pFormat, args);

	va_end(args);

	OutputDebugStringA(outBuf);
}

SERVICE_STATUS          ServiceStatus;
SERVICE_STATUS_HANDLE   ServiceStatusHandle;

VOID  ServiceStart (DWORD argc, LPTSTR *argv);
VOID  ServiceCtrlHandler (DWORD opcode);
void MainServiceThread(HANDLE hEvent);

SERVICE_TABLE_ENTRY   gDispatchTable[] =
    {
        {TEXT("PortServ"), (LPSERVICE_MAIN_FUNCTIONA) ServiceStart}, 
        {NULL, NULL}
    };

void NTServiceMain()
{
	StartServiceCtrlDispatcher(gDispatchTable);
}

void ServiceStart(DWORD argc, LPTSTR *argv)
{

	ServiceStatus.dwServiceType        = SERVICE_WIN32;
	ServiceStatus.dwCurrentState       = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP;
	ServiceStatus.dwWin32ExitCode      = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint         = 0;
	ServiceStatus.dwWaitHint           = 0;

	ServiceStatusHandle = RegisterServiceCtrlHandler(
		TEXT("PortServ"),
		(LPHANDLER_FUNCTION) ServiceCtrlHandler);

	if (ServiceStatusHandle == (SERVICE_STATUS_HANDLE) 0) {
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

	WaitForSingleObject(hInitialize, 1000);

	SetServiceStatus (ServiceStatusHandle, &ServiceStatus);

    return;
}

void MainServiceThread(HANDLE hEvent)
{
	HINSTANCE hModule;
	//
	if (hEvent == NULL)
		return;
	hModule = GetModuleHandle(NULL);
	if (!InitApplication(hModule)) {
		SetEvent(hEvent);
		return;
	}

	if (!InitInstance(hModule, 0)) {
		SetEvent(hEvent);
		return;
	}

	ServiceStatus.dwCurrentState       = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint         = 0;
	ServiceStatus.dwWaitHint           = 0;

	SetEvent(hEvent);

	MainMessageLoop(gpPSDialog->hDlg);

	ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
	ServiceStatus.dwCheckPoint         = 0;
	ServiceStatus.dwWaitHint           = 0;
	ServiceStatus.dwWin32ExitCode      = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;

	SetServiceStatus (ServiceStatusHandle, &ServiceStatus);
}

void ServiceStop()
{
	// stop message loop
	PostMessage(gpPSDialog->hDlg, WM_QUIT, 0, 0);

	delete gpPSDialog;

}

void ServiceCtrlHandler(DWORD Opcode)
{

	switch(Opcode)
	{
		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			break;
		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			break;
		case SERVICE_CONTROL_STOP:
		// stop PortServ service
			ServiceStop();
			//
			ServiceStatus.dwWin32ExitCode = 0;
			//MyServiceStatus.dwCurrentState  = SERVICE_STOP_PENDING;
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
			ServiceStatus.dwCheckPoint    = 0;
			ServiceStatus.dwWaitHint      = 0;

			if (!SetServiceStatus (ServiceStatusHandle, &ServiceStatus)) {
				SvcDebugPrintf("Failed to stop portserv Service\nerror code :%d\n", GetLastError());
			}
			return;
		case SERVICE_CONTROL_INTERROGATE:
		// Fall through to send current status.
			break;
		default:
			break;
	}

	// Send current status.
	SetServiceStatus (ServiceStatusHandle,  &ServiceStatus);
	return;
}


void NTServiceInstall()
{
	char ServicePath[MAX_PATH];
	SC_HANDLE   schSCManager;
	SC_HANDLE  schService;
	int       ret;

	ret = AppMessageBox(IDS_BEGININSTALLSERVICE,
						MB_YESNO | MB_ICONQUESTION);
	if (ret != IDYES)
		return;
	// set up path value
	GetModuleFileName(GetModuleHandle(NULL), ServicePath, MAX_PATH);
	strcat(ServicePath, " -service");

	schSCManager = OpenSCManager (NULL,                 // machine (NULL == local)
							  NULL,                 // database (NULL == default)
							  SC_MANAGER_ALL_ACCESS // access required
							  );

	schService = CreateService (schSCManager,          // SCManager database
								"PortServ",           // name of service
								"PortServ",           // name to display
								SERVICE_ALL_ACCESS,    // desired access
								SERVICE_WIN32_OWN_PROCESS, // service type
								SERVICE_AUTO_START,  // start type
								SERVICE_ERROR_NORMAL,  // error control type
								ServicePath,            // service's binary
								NULL,                  // no load ordering group
								NULL,                  // no tag identifier
								NULL,                  // no dependencies
								NULL,                  // LocalSystem account
								NULL                   // no password
								);
	if (schService != NULL) {
		ret = StartService (schService,
                0,
                NULL
                );
		AppMessageBox(IDS_SERVICEINSTALLED, MB_OK);
	} else {
		AppMessageBox(IDS_INSTSERVICEFAILED, MB_OK | MB_ICONINFORMATION);
	}
}

void NTServiceRemove()
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	int       ret;

	ret = AppMessageBox(IDS_SERVICEDELETING,
						MB_YESNO | MB_ICONQUESTION);
	if (ret != IDYES)
		return;

	schSCManager = OpenSCManager (NULL,                 // machine (NULL == local)
							  NULL,                 // database (NULL == default)
							  SC_MANAGER_ALL_ACCESS // access required
							  );

	schService = OpenService(schSCManager,
		"PortServ",
		SERVICE_ALL_ACCESS);
	if (schService != NULL) {
		SERVICE_STATUS lStatus;
		ControlService(schService, SERVICE_CONTROL_STOP,
				&lStatus);
		DeleteService(schService);
		AppMessageBox(IDS_SERVICEDELETED, MB_OK);
	} else {
		AppMessageBox(IDS_SERVICEDELETEFAILED, MB_OK | MB_ICONINFORMATION);
	}
}

////////////////////////////////////////////////////////////////////
// Windows 95/98 service portion
int ServiceInstall()
{
	int err;
	HKEY kRoot, kDest;
	unsigned long vtype;
	unsigned long cresult;
	char ServicePath[MAX_PATH];
	int       ret;

	ret = AppMessageBox(IDS_BEGININSTALLSERVICE,
						MB_YESNO | MB_ICONQUESTION);
	if (ret != IDYES)
		return FALSE;
	// set up path value
	GetModuleFileName(GetModuleHandle(NULL), ServicePath, MAX_PATH);
	strcat(ServicePath, " -service");

	err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		"Software\\Microsoft\\Windows\\CurrentVersion"
		, 0, /*KEY_READ*/ KEY_ALL_ACCESS
		, &kRoot);
	if (err == ERROR_SUCCESS) {
		err = RegCreateKeyEx( kRoot, "RunServices", NULL,
				"", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
				NULL, &kDest, &cresult);
		if (err == ERROR_SUCCESS) {
			err = RegQueryValueEx(kDest, "PortServ", NULL,
					&vtype, NULL, NULL);
			if (err != ERROR_SUCCESS) { // not installed yet
				err = RegSetValueEx(kDest, "PortServ", NULL,
					REG_SZ, (unsigned char *) ServicePath, strlen(ServicePath) + 1);
				if (err == ERROR_SUCCESS) {
					AppMessageBox(IDS_SERVICEINSTALLED, MB_OK);
					RegCloseKey(kDest);
					RegCloseKey(kRoot);
					return TRUE;
				}
			}
			RegCloseKey(kDest);
		}
		RegCloseKey(kRoot);
	}
	AppMessageBox(IDS_INSTSERVICEFAILED, MB_OK | MB_ICONINFORMATION);
	return FALSE;
}

void ServiceRemove()
{
	int err;
	HKEY kRoot, kDest;
	unsigned long vtype;
	unsigned long cresult;
	int       ret;
	char ServicePath[MAX_PATH];
	unsigned long lenPath = MAX_PATH;

	ret = AppMessageBox(IDS_SERVICEDELETING,
						MB_YESNO | MB_ICONQUESTION);
	if (ret != IDYES)
		return;

	err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		"Software\\Microsoft\\Windows\\CurrentVersion"
		, 0, /*KEY_READ*/ KEY_ALL_ACCESS
		, &kRoot);
	if (err == ERROR_SUCCESS) {
		err = RegCreateKeyEx( kRoot, "RunServices", NULL,
			"", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
			NULL, &kDest, &cresult);
		if (err == ERROR_SUCCESS) {
			err = RegQueryValueEx(kDest, "PortServ", NULL,
				&vtype, (unsigned char *) ServicePath, &lenPath);
			if (err == ERROR_SUCCESS) {// service has been installed
				KillPSS95(strtok(ServicePath, " "));
				err = RegDeleteValue(kDest, "PortServ");
				if (err == ERROR_SUCCESS) {
					AppMessageBox(IDS_SERVICEDELETED, MB_OK);
					RegCloseKey(kDest);
					RegCloseKey(kRoot);

					return ;
				}
			}
			RegCloseKey(kDest);
		}
		RegCloseKey(kRoot);
	}

	AppMessageBox(IDS_SERVICEDELETEFAILED, MB_OK | MB_ICONINFORMATION);
}

#include <tlhelp32.h>

typedef BOOL (WINAPI *THREADWALK)(HANDLE hSnapshot, 
    LPTHREADENTRY32 lpte);
typedef BOOL (WINAPI *PROCESSWALK)(HANDLE hSnapshot, 
    LPPROCESSENTRY32 lppe);
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags, 
    DWORD th32ProcessID);

void KillPSS95(char *pathService)
{
	CREATESNAPSHOT pCreateToolhelp32Snapshot = NULL;
	PROCESSWALK pProcess32First = NULL;
	PROCESSWALK pProcess32Next  = NULL;
	THREADWALK  pThread32First  = NULL;
	THREADWALK  pThread32Next   = NULL;
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};
	THREADENTRY32 te32 = {0};
	int ret;

	if (pathService == NULL)
		return;
//
	HANDLE hKernel = GetModuleHandle("KERNEL32.DLL");
	pCreateToolhelp32Snapshot = 
			(CREATESNAPSHOT)GetProcAddress((HINSTANCE) hKernel, "CreateToolhelp32Snapshot"); 
 	pProcess32First = 
			(PROCESSWALK)GetProcAddress((HINSTANCE) hKernel, "Process32First"); 
	pProcess32Next  = 
			(PROCESSWALK)GetProcAddress((HINSTANCE) hKernel, "Process32Next");
	pThread32First  = 
			(THREADWALK)GetProcAddress((HINSTANCE) hKernel, "Thread32First");
	pThread32Next   = (THREADWALK)GetProcAddress((HINSTANCE) hKernel, "Thread32Next");
	if (!pCreateToolhelp32Snapshot || !pProcess32First ||
		!pThread32First || !pThread32Next)
		return;
//
	hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
	if (hProcessSnap == (HANDLE)-1)
		return;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	ret = pProcess32First(hProcessSnap, &pe32);
	while (ret) {
		if (stricmp(pathService, pe32.szExeFile) == 0) {
			if (GetCurrentProcessId() != pe32.th32ProcessID) {
				break;
			}
		}
        ret = pProcess32Next(hProcessSnap, &pe32);
	}

	te32.dwSize = sizeof(THREADENTRY32);

	ret = pThread32First(hProcessSnap, &te32);
	while (ret) {
		if (te32.th32OwnerProcessID == pe32.th32ProcessID) {
			PostThreadMessage(te32.th32ThreadID, WM_QUIT, 0, 0);
		}
		ret = pThread32Next(hProcessSnap, &te32);
	}
    CloseHandle (hProcessSnap);
    return ;
}

