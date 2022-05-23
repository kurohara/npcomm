// instdrv.c
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
#include "instdrv.h"
#include <stdio.h>

#define SERVICE_NAME	"PCOMM"
#define SERVICE_PATH	"\\pserial.sys"

static char DriverPath[256];

int SetParameters(drvInfo *pInfo)
{
	HKEY kRoot;
	HKEY kService;
	HKEY kParameters;
	int ret, result;
	int i;

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				"SYSTEM\\CurrentControlSet\\Services",
				0,
				KEY_READ,
				&kRoot);
	
	if (ret != ERROR_SUCCESS) {
		return FALSE;
	}
	
	ret = RegOpenKeyEx(kRoot,
				SERVICE_NAME,
				0,
				KEY_ALL_ACCESS,
				&kService);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(kRoot);
		return FALSE;
	}
	ret = RegCreateKeyEx(kService,
				"Parameters",
				0, // Reserved
				"", // class name
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				NULL, // security addribute
				&kParameters,
				&result);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(kRoot);
		RegCloseKey(kService);
		return FALSE;
	}

	if (pInfo->nPCOMM > 0) {
		i = 0;
		ret = RegSetValueEx(kParameters,
					"PCOMM1",
					0,
					REG_DWORD,
					(char *) &(pInfo->pinfo[i].portNum),
					sizeof(int));
		ret = RegSetValueEx(kParameters,
					"PCOMM_NAME1",
					0,
					REG_SZ,
					pInfo->pinfo[i].pcomName,
					strlen(pInfo->pinfo[i].pcomName));
		ret = RegSetValueEx(kParameters,
					"DPCOMM_NAME1",
					0,
					REG_SZ,
					pInfo->pinfo[i].dpcomName,
					strlen(pInfo->pinfo[i].dpcomName));
	}
	if (pInfo->nPCOMM > 1) {
		i++;
		ret = RegSetValueEx(kParameters,
					"PCOMM2",
					0,
					REG_DWORD,
					(char *) &(pInfo->pinfo[i].portNum),
					sizeof(int));
		ret = RegSetValueEx(kParameters,
					"PCOMM_NAME2",
					0,
					REG_SZ,
					pInfo->pinfo[i].pcomName,
					strlen(pInfo->pinfo[i].pcomName));
		ret = RegSetValueEx(kParameters,
					"DPCOMM_NAME2",
					0,
					REG_SZ,
					pInfo->pinfo[i].dpcomName,
					strlen(pInfo->pinfo[i].dpcomName));
	}
	if (pInfo->nPCOMM > 2) {
		i++;
		ret = RegSetValueEx(kParameters,
					"PCOMM3",
					0,
					REG_DWORD,
					(char *) &(pInfo->pinfo[i].portNum),
					sizeof(int));
		ret = RegSetValueEx(kParameters,
					"PCOMM_NAME3",
					0,
					REG_SZ,
					pInfo->pinfo[i].pcomName,
					strlen(pInfo->pinfo[i].pcomName));
		ret = RegSetValueEx(kParameters,
					"DPCOMM_NAME3",
					0,
					REG_SZ,
					pInfo->pinfo[i].dpcomName,
					strlen(pInfo->pinfo[i].dpcomName));
	}

	RegCloseKey(kRoot);
	RegCloseKey(kService);
	RegCloseKey(kParameters);
	return TRUE;
}

int GetParameters(drvInfo *pInfo)
{
	HKEY kParameters;
	int ret;
	int i;
	char kName[256];
	int vSize;

	sprintf(kName, 
		"SYSTEM\\CurrentControlSet\\Services\\%s\\Parameters",
		SERVICE_NAME);

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				kName,
				0,
				KEY_READ,
				&kParameters);
	
	if (ret != ERROR_SUCCESS) {
		return FALSE;
	}

	vSize = sizeof (pInfo->pinfo[i].portNum);
	i = 0;
	ret = RegQueryValueEx(kParameters,
				"PCOMM1",
				NULL,
				NULL,
				(char *) &(pInfo->pinfo[i].portNum),
				&vSize);
	if (ret == ERROR_SUCCESS)
		pInfo->nPCOMM = 1;
	else {
		RegCloseKey(kParameters);
		return FALSE;
	}

	vSize = sizeof (pInfo->pinfo[i].pcomName);
	ret = RegQueryValueEx(kParameters,
				"PCOMM_NAME1",
				NULL,
				NULL,
				pInfo->pinfo[i].pcomName,
				&vSize);
	vSize = sizeof (pInfo->pinfo[i].pcomName);
	ret = RegQueryValueEx(kParameters,
				"DPCOMM_NAME1",
				NULL,
				NULL,
				pInfo->pinfo[i].dpcomName,
				&vSize);
	///
	i++;
	vSize = sizeof (pInfo->pinfo[i].portNum);
	ret = RegQueryValueEx(kParameters,
				"PCOMM2",
				NULL,
				NULL,
				(char *) &(pInfo->pinfo[i].portNum),
				&vSize);
	if (ret == ERROR_SUCCESS)
		pInfo->nPCOMM = 2;
	else {
		RegCloseKey(kParameters);
		return TRUE;
	}

	vSize = sizeof (pInfo->pinfo[i].pcomName);
	ret = RegQueryValueEx(kParameters,
				"PCOMM_NAME2",
				NULL,
				NULL,
				pInfo->pinfo[i].pcomName,
				&vSize);
	vSize = sizeof (pInfo->pinfo[i].dpcomName);
	ret = RegQueryValueEx(kParameters,
				"DPCOMM_NAME2",
				NULL,
				NULL,
				pInfo->pinfo[i].dpcomName,
				&vSize);
	///
	i++;
	vSize = sizeof (pInfo->pinfo[i].portNum);
	ret = RegQueryValueEx(kParameters,
				"PCOMM3",
				NULL,
				NULL,
				(char *) &(pInfo->pinfo[i].portNum),
				&vSize);
	if (ret == ERROR_SUCCESS)
		pInfo->nPCOMM = 3;
	else {
		RegCloseKey(kParameters);
		return TRUE;
	}

	vSize = sizeof (pInfo->pinfo[i].pcomName);
	ret = RegQueryValueEx(kParameters,
				"PCOMM_NAME3",
				NULL,
				NULL,
				pInfo->pinfo[i].pcomName,
				&vSize);
	vSize = sizeof (pInfo->pinfo[i].dpcomName);
	ret = RegQueryValueEx(kParameters,
				"DPCOMM_NAME3",
				NULL,
				NULL,
				pInfo->pinfo[i].dpcomName,
				&vSize);
	///
	RegCloseKey(kParameters);
	return TRUE;
}

int GetLastComId()
{
	HKEY kRoot;
	int ret;
	char className[256];
	int classLen, nSubKeys, maxSubKeyLen;
	int maxClassLen, nValues, maxValueLen;
	int maxValueNameLen, securityDesc;
	FILETIME lastWriteTime;
	int i;
	char valueName[256], value[256];
	int valueLen,valueNameLen;
	int type;
	int maxComId;

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		"HARDWARE\\DEVICEMAP\\SERIALCOMM",
		0,
		KEY_READ | KEY_QUERY_VALUE,
		&kRoot);
	if (ret != ERROR_SUCCESS) 
		return -1;

	classLen = sizeof (className);
	ret = RegQueryInfoKey(kRoot, 
		className,
		&classLen,
		NULL,
		&nSubKeys,
		&maxSubKeyLen,
		&maxClassLen,
		&nValues,
		&maxValueNameLen,
		&maxValueLen,
		&securityDesc,
		&lastWriteTime);
	
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(kRoot);
		return -1;
	}

	maxComId = 0;
//	for (i=0;i<=nValues;i++) {
	i = 0;
	while(1) {
		int curComId;
		valueNameLen = sizeof (valueName);
		valueLen = sizeof (value);
		ret = RegEnumValue(kRoot,
					i,
					valueName,
					&valueNameLen,
					NULL,
					&type,
					value,
					&valueLen);
		i++;
		if (ret == ERROR_NO_MORE_ITEMS)
			break;
		if (ret != ERROR_SUCCESS)
			continue;
		sscanf(value, "COM%d", &curComId);
		if (curComId > maxComId)
			maxComId = curComId;
	}

	RegCloseKey(kRoot);
	return maxComId;
}

int GetDriverStatus(int *bRunning, char *pPath, int *bAuto)
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	int ret, err;
	SERVICE_STATUS status;
	QUERY_SERVICE_CONFIG *pConfig, tmpConfig;
	int sizeConfig;

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
                              SERVICE_NAME,
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

int CheckDriverFile()
{
	char ServicePath[256];
	HANDLE hFile;

//	GetCurrentDirectory(256, ServicePath);
	strcpy(ServicePath, DriverPath);
	strcat(ServicePath, SERVICE_PATH);
	hFile = CreateFile(ServicePath,
				0,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	else {
		CloseHandle(hFile);
		return TRUE;
	}
}

int GetDriverFileName(char *pName)
{
	strcpy(pName, DriverPath);
	strcat(pName, SERVICE_PATH);
	return TRUE;
}

int SetDriverPath(char *pDir)
{

	if (pDir == NULL) {
		GetCurrentDirectory(256, DriverPath);
	} else {
		strcpy(DriverPath, pDir);
	}
	return TRUE;
}

int InstDrv(int autostart)
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	char ServicePath[256];
	int ret;
	int startType;

	//GetCurrentDirectory(256, ServicePath);
	strcpy(ServicePath, DriverPath);
	strcat(ServicePath, SERVICE_PATH);

	schSCManager = OpenSCManager (
			NULL,                 // machine (NULL == local)
			NULL,                 // database (NULL == default)
			SC_MANAGER_ALL_ACCESS // access required
		);

	if (schSCManager == NULL) {
		ret = GetLastError();
		return FALSE;
	}
	if (autostart)
		startType = SERVICE_AUTO_START;
	else
		startType = SERVICE_DEMAND_START;

    schService = CreateService (
			schSCManager,          // SCManager database
            SERVICE_NAME,           // name of service
            SERVICE_NAME,           // name to display
            SERVICE_ALL_ACCESS,    // desired access
            SERVICE_KERNEL_DRIVER, // service type
            startType,  // start type
            SERVICE_ERROR_NORMAL,  // error control type
            ServicePath,            // service's binary
            NULL,                  // no load ordering group
            NULL,                  // no tag identifier
            NULL,                  // no dependencies
            NULL,                  // LocalSystem account
            NULL                   // no password
            );

	if (schService == NULL) {
		ret = GetLastError();
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	CloseServiceHandle(schSCManager);

	CloseServiceHandle(schService);

	return TRUE;

}

int DeinstDrv()
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	int ret;

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
                              SERVICE_NAME,
                              SERVICE_ALL_ACCESS
                              );

	if (schService == NULL) {
		ret = GetLastError();
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	DeleteService(schService);

	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	return TRUE;
}

int StartDrv()
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	int ret;

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
                              SERVICE_NAME,
                              SERVICE_ALL_ACCESS
                              );

	if (schService == NULL) {
		ret = GetLastError();
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	ret = StartService (schService,    // service identifier
					0,             // number of arguments
					NULL           // pointer to arguments
					);

	if (ret != TRUE) {
		ret = GetLastError();
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return FALSE;
	}
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	return TRUE;

}

int StopDrv()
{
    SC_HANDLE   schSCManager;
    SC_HANDLE  schService;
	SERVICE_STATUS status;
	int ret;

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
                              SERVICE_NAME,
                              SERVICE_ALL_ACCESS
                              );

	if (schService == NULL) {
		ret = GetLastError();
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	ret = ControlService(schService,
		SERVICE_CONTROL_STOP,
		&status);

	if (ret != TRUE) {
		ret = GetLastError();
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return FALSE;
	}
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	return TRUE;

}
