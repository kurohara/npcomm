/***********************************************************************
    TcpMgr.h
	Tcp/ip connection manager for PortShare server program.

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
#ifndef _TCPMGR_H
#define _TCPMGR_H
#include "sock.h"

#define DEF_SERVICEPORT		2345

class MgrListHead : public AnyList
{
	CRITICAL_SECTION listCrst;
	static AnyList *pEnumCurrent;
public:
	MgrListHead()
	{
		InitializeCriticalSection(&listCrst);
	}
	~MgrListHead()
	{
		DeleteCriticalSection(&listCrst);
	}
	void insertIt(AnyList *pL)
	{
		EnterCriticalSection(&listCrst);
		AnyList::insertIt(pL);
		LeaveCriticalSection(&listCrst);
	}
	void removeIt(AnyList *pL)
	{
		EnterCriticalSection(&listCrst);
		pL->removeMe();
		LeaveCriticalSection(&listCrst);
	}
	void startEnum()
	{
		EnterCriticalSection(&listCrst);
		if (pEnumCurrent == NULL) {
			pEnumCurrent = this;
		}
	}
	AnyList *enumNext()
	{
		if (pEnumCurrent != NULL) {
			pEnumCurrent = pEnumCurrent->getNext();
			return pEnumCurrent;
		}
		return NULL;
	}
	void endEnum()
	{
		if (pEnumCurrent) {
			pEnumCurrent = NULL;
		}
		LeaveCriticalSection(&listCrst);
	}
};

class AuthManager : public PortAuth
{
	static int dId;
public:
	int Authenticate(char *pHostName, char *pUserName, char *pPortName);
	void Leave(int iid);
};

class TcpManager
{
	SOCKET hSockSt;
	SOCKET hSockDg;
	TcpManager();
	int servicePortNum;
	static int bStarted;
	HANDLE hManagerThread;
public:
	MgrListHead Clients;
	PortAuth *pAuth;
	static TcpManager *pMe;
	static int StartManager(int servicePort);
	static TcpManager *GetManager();
	static void StopManager();
	~TcpManager();

	int Initialize();
	int Run();
	//
private:
	void StartClient(SOCKET sh);
	SOCKET AcceptConnection();
	SOCKET ConnectToClient(SOCKADDR_IN *dest);
	SOCKET DGProc();
};

#endif _TCPMGR_H
