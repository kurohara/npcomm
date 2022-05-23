/* V2Mgr.h */
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
#ifndef _V2MGR_H
#define _V2MGR_H

#define MAX_TRANSPORTS 10
#define INIFILEPATH "NPCOMM.ini"
#define APPNAME "NPCOMM"
#define APPVERSION "NPCOMM ver 2.0 beta"

#define NPVERSIONNUM 200

#define WARNCOUNT 2

#define SECTION_GENERAL		"General"
#define KEY_USERNAME		"Username"
#define KEY_CHANGEFOCUS		"ChangeFocusOnOpen"
#define KEY_PASSNUMH		"KeyValue1"
#define KEY_PASSNUML		"KeyValue2"
#define KEY_TRANSPORTNAME	"DefTransport"
#define KEY_ISDEBUG			"IsDebug"

#include "pcommxs.h"
typedef struct AdmTransportRec {
	int id;
	PCommTransport *pTransport;
	char *pName;
	HINSTANCE hModule;
} AdmTransport;

#include "pcommMgr.h"
#include "pcommdrv.h"

class V2Manager : public PCommManager
{
	char IniFilePath[MAX_PATH];
	char *pcBuf, *pcHead;
	int nextTransportId;
	CRITICAL_SECTION dspCrst;
	HANDLE hMainThread;
public:
	V2Manager(HINSTANCE hInstance, int iMgr);
	~V2Manager();
	static int numManagers;
	int mgrIndex;
	char MgrName[MAX_PATH];
	char LocalPortName[256];
	int bChangeFocus;
	int bOpened;
	int bDoRun;
	int idCurrentTransport;
	int nTransports;
	AdmTransport dTransports[MAX_TRANSPORTS];
	PCommTransport *currentTransport;
	char pathHelpFile[MAX_PATH];

	PCommDevice *pDevice;
	// pointer to Main dialog frame
	// MainFrame also has pointer to me.
	// I dislikes this kind of cross reference.
	void *pMainDialog;

	// implementations
	// profile functions
	virtual int GetProfileString(char *Section, char *Key, char *Value, int maxLen);
	virtual int GetProfileInt(char *Section, char *Key, int DefValue);
	virtual int WriteProfileString(char *Section, char *Key, char *Value);
	virtual int WriteProfileInt(char *Section, char *Key, int Value);
	// transport support functions
	virtual int RegisterTransport(HINSTANCE hModule, PCommTransport *tr, char *name);
	virtual int DeleteTransport(int trId);
	//
	virtual int Query(int Command, void *data);
	virtual int Control(int Command, void *data);
	//
	virtual int Dispatch(int Command, int DataSize, void *pData);
	//
	int Initialize();
	int InitializeTransports();
	int InitializeDevice();
	//
	int TransportSelect(int trId);
	void DoLocalPortChange();
	void OpenHelpPage(int pageId);
	void ExitManager();
	PCommTransport *GetTransportById(int id);

	void Run();
	int StartMainManagerThread(void);
private:

};

class tmpDevice : public PCommDevice
{
public:
	virtual int DeviceInitialize(char *ComName, int index) { strcpy(ComName, "COM?"); maxSendSize = 256; return TRUE; };
	virtual void DeviceRelease(void) { return; };
	//
	virtual void ChangeCommName(void) { return; };
	//
	virtual int GetPCommCommand(int Timeout, int *Command, int *DataSize, void *Data);
	virtual int CommandAck(int Kind) { return TRUE; };
	//
	virtual int Notify(int Kind, EventData *pE) { return TRUE; };
	virtual int WriteData(int Size, char *pBuf) { return TRUE; };
	virtual int SetCommConfig(CommConfig *pDcb) { return TRUE; };
	virtual int SetModemStat(int modemStat) { return TRUE; };
	virtual int Disconnect(void) { return TRUE;};

} ;

#endif _V2MGR_H

