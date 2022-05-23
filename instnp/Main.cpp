// Main.cpp
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "Dialog.h"
#include <process.h>

int AppMessageBox(unsigned int resid, unsigned int mask);
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
int MainMessageLoop(HWND mainWnd);
int NTServiceInstall(int index, char *pExArgs);
int NTServiceRemove(int index);

OSVERSIONINFO gOsVersion;
char gCurrentDirectory[MAX_PATH];
MyDialog *gpDialog;
#define APPNAME "Instnp"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	gOsVersion.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&gOsVersion);
	if (gOsVersion.dwPlatformId != VER_PLATFORM_WIN32_NT) {
		AppMessageBox(IDS_OSERR, MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if (!InitApplication(hInstance)) {
		return FALSE;
	}

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	return MainMessageLoop(gpDialog->hDlg);
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

	return (msg.wParam);
}

int CheckNpcommRunning()
{
	static HANDLE npcommMutex;
	int mState;
	char mutexName[MAX_PATH];
	int i;
	int ret = FALSE;

	for (i=0;i<3;i++) {
		if (i > 0)
			sprintf(mutexName, "Kurohara_NPCOMM%d", i);
		else
			strcpy(mutexName, "Kurohara_NPCOMM");
		npcommMutex = CreateMutex(NULL, FALSE, mutexName);
		mState = WaitForSingleObject(npcommMutex, 1);
		if (mState == WAIT_TIMEOUT) {
			ret = TRUE;
		} else {
			ReleaseMutex(npcommMutex);
		}
	}

	return ret;
}

int CheckNpcommExist()
{
	char ServicePath[MAX_PATH];

	// set up path value
	strcpy(ServicePath, gCurrentDirectory);
	strcat(ServicePath, "\\npcomm.exe");

	HANDLE hNpcomm = CreateFile(ServicePath, 
						/*GENERIC_READ */0,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

	if (hNpcomm == INVALID_HANDLE_VALUE)
		return FALSE;

	return TRUE;
}

int CheckDriverExist()
{
	char ServicePath[MAX_PATH];

	// set up path value
	strcpy(ServicePath, gCurrentDirectory);
	strcat(ServicePath, "\\pserial.sys");
	HANDLE hNpcomm = CreateFile(ServicePath, 
						/*GENERIC_READ */0,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

	if (hNpcomm == INVALID_HANDLE_VALUE)
		return FALSE;

	return TRUE;
}

BOOL InitApplication(HINSTANCE hInstance)
{
	return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	GetCurrentDirectory(MAX_PATH, gCurrentDirectory);

	gpDialog = new MyDialog;

	gpDialog->Create(hInstance);

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

////////////////////////////////////////////////////////////////////
// Windows NT service portion

int NTServiceInstall(int index, char *pExArguments)
{
	char ServicePath[MAX_PATH];
	char ServiceName[MAX_PATH];
	char dependNames[MAX_PATH];
	char *pDep;
	SC_HANDLE   schSCManager;
	SC_HANDLE  schService;
	int       ret;

	memset(dependNames, 0, MAX_PATH);
	pDep = dependNames;
	strcpy(pDep, "PCOMM");
	pDep += strlen(pDep) + 1;
	strcpy(pDep, "RPCSS");
	pDep += strlen(pDep) + 1;
	strcpy(pDep, "NTLMSSP");

	// set up path value
	if (index > 0) {
		sprintf(ServicePath, "%s\\npcomm.exe  -service -i%d", gCurrentDirectory, index);
		sprintf(ServiceName, "NPCOMM%d", index);
	} else {
		sprintf(ServicePath, "%s\\npcomm.exe  -service", gCurrentDirectory);
		strcpy(ServiceName, "NPCOMM");
	}

	strcat(ServicePath, " ");
	strcat(ServicePath, pExArguments);

	schSCManager = OpenSCManager (NULL,                 // machine (NULL == local)
							  NULL,                 // database (NULL == default)
							  SC_MANAGER_ALL_ACCESS // access required
							  );

	schService = CreateService (schSCManager,          // SCManager database
								ServiceName,           // name of service
								ServiceName,           // name to display
								SERVICE_ALL_ACCESS,    // desired access
								SERVICE_WIN32_OWN_PROCESS, // service type
								SERVICE_AUTO_START,  // start type
								SERVICE_ERROR_NORMAL,  // error control type
								ServicePath,            // service's binary
								NULL,                  // no load ordering group
								NULL,                  // no tag identifier
								dependNames,           // no dependencies
								NULL,                  // LocalSystem account
								NULL                   // no password
								);
	if (schService != NULL) {
#if 0
		ret = StartService (schService,
                0,
                NULL
                );
#endif
		ret = TRUE;
	} else {
		ret = FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return ret;
}

int NTServiceRemove(int index)
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	char ServiceName[MAX_PATH];
	int       ret;

	if (index > 0) {
		sprintf(ServiceName, "NPCOMM%d", index);
	} else {
		strcpy(ServiceName, "NPCOMM");
	}

	schSCManager = OpenSCManager (NULL,                 // machine (NULL == local)
							  NULL,                 // database (NULL == default)
							  SC_MANAGER_ALL_ACCESS // access required
							  );

	schService = OpenService(schSCManager,
		ServiceName,
		SERVICE_ALL_ACCESS);

	if (schService != NULL) {
		SERVICE_STATUS lStatus;
		ControlService(schService, SERVICE_CONTROL_STOP,
				&lStatus);
		DeleteService(schService);
		ret = TRUE;
	} else {
		ret = FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return ret;
}

int GetServiceStatus(int index, int *bRunning, char *pPath, int *bAuto)
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	int ret, err;
	SERVICE_STATUS status;
	QUERY_SERVICE_CONFIG *pConfig, tmpConfig;
	unsigned long sizeConfig;
	char serviceName[MAX_PATH];

	if (index > 0)
		sprintf(serviceName, "NPCOMM%d", index);
	else
		strcpy(serviceName, "NPCOMM");

	schSCManager = OpenSCManager (
			NULL,                 // machine (NULL == local)
			NULL,                 // database (NULL == default)
			SC_MANAGER_ALL_ACCESS // access required
		);

	if (schSCManager == NULL) {
		ret = GetLastError();
		return FALSE;
	}
    schService = OpenService (schSCManager,
                              serviceName,
                              SERVICE_ALL_ACCESS
                              );

	if (schService == NULL) {
		ret = GetLastError();
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	ret = QueryServiceStatus(schService, &status);
	if (ret != TRUE) {
		err = GetLastError();
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return FALSE;
	}

	pConfig = &tmpConfig;
	ret = QueryServiceConfig(schService, pConfig, 0, &sizeConfig);
	pConfig = (QUERY_SERVICE_CONFIG *) malloc(sizeof (QUERY_SERVICE_CONFIG) + sizeConfig);
	ret = QueryServiceConfig(schService, pConfig, sizeConfig, &sizeConfig);
	if (pPath != NULL)
		strcpy(pPath, pConfig->lpBinaryPathName);
	*bAuto = pConfig->dwStartType == SERVICE_AUTO_START ? TRUE : FALSE;
	free(pConfig);
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);
	*bRunning = status.dwCurrentState == SERVICE_RUNNING ? TRUE : FALSE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////
