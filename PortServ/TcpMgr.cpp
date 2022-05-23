/***********************************************************************
    TcpMgr.cpp
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
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include "TcpMgr.h"

extern "C" int ChangeAutoDial(BOOL bAuto);
void RefreshDisplay();


int InitializeSockSystem()
{
	WSADATA WsaData;
	WORD wVersionRequested;
	SOCKET tmpSh;
	int err;

	tmpSh = socket(PF_INET, SOCK_STREAM, 0);
	if (tmpSh == INVALID_SOCKET) {
		if (WSAGetLastError() != WSANOTINITIALISED) {
			return FALSE;
		}
	} else {
		closesocket(tmpSh);
		return TRUE;
	}

	wVersionRequested = MAKEWORD( 2, 0 );
	err = WSAStartup(wVersionRequested,&WsaData);
	// though it seems no need to retry....
	if (err!=0) {
		WSACleanup(); 
		wVersionRequested = MAKEWORD(1, 1); 
		err = WSAStartup(wVersionRequested,&WsaData);
	}

	return TRUE;
}

unsigned int 
_stdcall ManagerThread(TcpManager *pMgr)
{
	try {
		pMgr->Run();
	}
	catch (...) {
	}
	_endthreadex(TRUE);
	return TRUE;
}


TcpManager *TcpManager::pMe = NULL;
int TcpManager::bStarted = FALSE;
AnyList *MgrListHead::pEnumCurrent= NULL;

TcpManager *TcpManager::GetManager()
{
	if (pMe == NULL)
		pMe = new TcpManager;
	return pMe;
}

int TcpManager::StartManager(int servicePort)
{
	unsigned int thId;

	if (bStarted)
		return FALSE;
	(void) TcpManager::GetManager();
	pMe->servicePortNum = servicePort;
	if (!pMe->Initialize())
		return FALSE;
	pMe->hManagerThread = (HANDLE) _beginthreadex(
					NULL, 0,
					(unsigned int (_stdcall *)(void *)) ManagerThread,
					(void *) pMe,
					0, &thId);
	bStarted = TRUE;
	return TRUE;

}

void TcpManager::StopManager()
{
	int ret;
	shutdown(pMe->hSockSt, 2);
	closesocket(pMe->hSockSt);
	ret = WaitForSingleObject(pMe->hManagerThread, 10000 /*INFINITE*/);
	if (ret == WAIT_TIMEOUT) {
		TerminateThread(pMe->hManagerThread, 0);
	}
	bStarted = FALSE;
	closesocket(pMe->hSockDg);
	CloseHandle(pMe->hManagerThread);
	// may need to close all client session,
	// or it may need to be remain connected.
}

void ClientThread(SOCKET sh)
{
	TcpProtocol *pNewClient;

	pNewClient = TcpProtocol::Create(sh);
	if (pNewClient != NULL) {
		// setup client name anyway.
		SOCKADDR_IN sin;
		int nameLen = sizeof (SOCKADDR_IN);
		getpeername(sh, (SOCKADDR *) &sin, &nameLen);
		strcpy(pNewClient->clientName, inet_ntoa(sin.sin_addr));
		//
		TcpManager::pMe->Clients.insertIt(pNewClient);

		RefreshDisplay();
		try {
			pNewClient->StartComm(TcpManager::pMe->pAuth);
		}
		catch (...) {
		}
		TcpManager::pMe->Clients.removeIt(pNewClient);
		RefreshDisplay();
		delete pNewClient;
	}
	shutdown(sh, 2);
	closesocket(sh);
	_endthread();
}

TcpManager::TcpManager()
{
	pAuth = new AuthManager;
}

TcpManager::~TcpManager()
{
	delete pAuth;
}

int TcpManager::Initialize()
{
	SOCKADDR_IN localAddr;
	unsigned long bFlag;
	int err;
	// for datagram socket
	hSockDg = socket(PF_INET, SOCK_DGRAM, 0);
	//
	ZeroMemory (&localAddr, sizeof (localAddr));
	localAddr.sin_port = htons (servicePortNum);
	localAddr.sin_family = AF_INET;
	err = bind (hSockDg, (PSOCKADDR) & localAddr, sizeof (localAddr));
	if (err == SOCKET_ERROR) {
		return FALSE;
	}
	
	// for stream socket
	hSockSt = socket(PF_INET, SOCK_STREAM, 0);
	ZeroMemory (&localAddr, sizeof (localAddr));
	localAddr.sin_port = htons (servicePortNum);
	localAddr.sin_family = AF_INET;
	err = bind (hSockSt, (PSOCKADDR) & localAddr, sizeof (localAddr));
	if (err == SOCKET_ERROR) {
		return FALSE;
	}
	listen(hSockSt, 5);
	bFlag = 1;
	ioctlsocket(hSockSt, FIONBIO, &bFlag);
	//
	//


	return TRUE;
}

int TcpManager::Run()
{
	struct timeval tm;
	SOCKET snew;
	int ret;
	fd_set rset;

	while (1) {
		FD_ZERO(&rset);
		FD_SET(hSockSt, &rset);
		FD_SET(hSockDg, &rset);

		ret = select(2, &rset, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR || ret == 0) {
			return FALSE;
		}
		// for PortServ protocols part
		if (FD_ISSET(hSockDg, &rset)) {
			snew = DGProc();
			if (snew != NULL) {
				StartClient(snew);
			}
		}
		if (FD_ISSET(hSockSt, &rset)) {
			snew = AcceptConnection();
			if (snew != NULL) {
				StartClient(snew);
			}
		}
		//
		// if other port has been listened, ....
		//
	}

	return FALSE;
}

void TcpManager::StartClient(SOCKET snew)
{
	_beginthread((void (*)(void *)) ClientThread,
					0,
					(void *) snew);

}

SOCKET TcpManager::AcceptConnection()
{
	SOCKET snew;
	unsigned long bFlag;
	// setting "blocking mode".
	bFlag = 0;
	ioctlsocket(hSockSt, FIONBIO, &bFlag);
	//
	snew = accept(hSockSt, NULL, NULL);
	if (snew == INVALID_SOCKET) {
		return NULL;
	}
	// setting "blocking mode".
	bFlag = 0;
	ioctlsocket(snew, FIONBIO, &bFlag);
	//
	return snew;
}


SOCKET TcpManager::ConnectToClient(SOCKADDR_IN *dest)
{
	unsigned long bFlag;
	int ret;
	SOCKET snew;
	SOCKADDR_IN localAddr;

	snew = socket(PF_INET, SOCK_STREAM, 0);
	if (snew == INVALID_SOCKET) {
		return NULL;
	}
	ZeroMemory (&localAddr, sizeof (localAddr));
	localAddr.sin_port = 0;
	localAddr.sin_family = AF_INET;
	ret = bind (snew, (PSOCKADDR) & localAddr, sizeof (localAddr));
	if (ret == SOCKET_ERROR) {
		closesocket(snew);
		return NULL;
	}

	// setting "non blocking mode".
	bFlag = 1;
	ioctlsocket(snew, FIONBIO, &bFlag);
	//

	// if this machine has set up to connect to ISP,
	// connect() will cause bring up DUN dialog.
	//
	int oldAutoDial;
	oldAutoDial = ChangeAutoDial(FALSE);
	ret = connect (snew, (PSOCKADDR)  dest, sizeof (SOCKADDR_IN));
	ChangeAutoDial(oldAutoDial);

	if (ret == SOCKET_ERROR) {
		if ((ret = WSAGetLastError()) == WSAEWOULDBLOCK) {
			fd_set wrfd;
			struct timeval tmval;
			tmval.tv_sec = 0;
			tmval.tv_usec = 500000; // 500 * 1000 micro sec wait time

			FD_ZERO(&wrfd);
			FD_SET(snew, &wrfd);
			ret = select(1, NULL, &wrfd, NULL, &tmval);
			if (ret == 0 || ret == SOCKET_ERROR) {
				closesocket(snew);
				return NULL;
			} else {
				// force set to "blocking mode".
				bFlag = 0;
				ioctlsocket(snew, FIONBIO, &bFlag);
				return snew;
			}
		} else {
			closesocket(snew);
			return NULL;
		}
	}
	else {
		// force set to "blocking mode".
		bFlag = 0;
		ioctlsocket(snew, FIONBIO, &bFlag);
		return snew;
	}

	closesocket(snew);
	return NULL;
}

SOCKET TcpManager::DGProc()
{
	int fromLen;
	SOCKADDR_IN sin, to;
	int ret;
	UDPCommand cPack, rPack;
	SOCKET snew;

	fromLen = sizeof(sin);
	ret = recvfrom(hSockDg, (char *) &cPack, 
				sizeof(cPack), 
				0,
				(PSOCKADDR) &sin,
				&fromLen);

	if (ret == SOCKET_ERROR || ret == 0) {
		return NULL;
	}

	switch (cPack.command) {
	case UDP_CONNECTREQUEST:
		// new connection
		//
		ZeroMemory(&to, sizeof(SOCKADDR_IN));
		CopyMemory(&to.sin_addr, &sin.sin_addr,
				sizeof(sin.sin_addr));
		to.sin_family = AF_INET;
		// port number will be got from UDP message in future version.
		to.sin_port = htons (servicePortNum);
		//
		// connect to client
		snew = ConnectToClient(&to);
		if (snew == NULL) {
			break;
		}
		return snew;
	case UDP_AYT: {
		SOCKADDR_IN destAddr;

		ZeroMemory(&destAddr, sizeof(SOCKADDR_IN));
		CopyMemory(&destAddr, &sin,	sizeof(sin));
		//
		rPack.command = UDP_ACK;
		fromLen = sizeof(destAddr);
		sendto(hSockDg, (char *) &rPack, sizeof (rPack), MSG_DONTROUTE ,
						(SOCKADDR *) &destAddr, fromLen);
				  }
		break;
	case UDP_STATUSREQ: {
		SOCKADDR_IN destAddr;
		ServerStatus *pStatus = (ServerStatus *) &rPack.data;
		pStatus->type = STTYPE_CLIENTS;
		rPack.command = UDP_STATUS;
		int cCount;
		TcpProtocol *pCur;
		ZeroMemory(&destAddr, sizeof(SOCKADDR_IN));
		CopyMemory(&destAddr, &sin,	sizeof(sin));
		//
		cCount = 0;
		Clients.startEnum();
		pCur = (TcpProtocol *) Clients.enumNext();
		while (pCur != NULL) {
			_snprintf(pStatus->status, sizeof (pStatus->status) , "%s,%s",
				pCur->clientName, pCur->portName);

			fromLen = sizeof(destAddr);
			sendto(hSockDg, (char *) &rPack, sizeof (rPack), MSG_DONTROUTE ,
							(SOCKADDR *) &destAddr, fromLen);
			cCount ++;
			pCur = (TcpProtocol *) Clients.enumNext();
		}
		Clients.endEnum();
		if (cCount == 0) {
			strcpy(pStatus->status, "no clients");

			fromLen = sizeof(destAddr);
			sendto(hSockDg, (char *) &rPack, sizeof (rPack), MSG_DONTROUTE ,
							(SOCKADDR *) &destAddr, fromLen);
		}
						}
		break;
	default:
		break;
	}
	return NULL;
}

// the only "sticky" portion.
#include "PSDialog.h"
extern PortServDlg *gpPSDialog;
//
void RefreshDisplay()
{
	TcpProtocol *pCur;
	char dispString[MAX_PATH];

	TcpManager::pMe->Clients.startEnum();
	gpPSDialog->ClearConnectionEntries();
	pCur = (TcpProtocol *) TcpManager::pMe->Clients.enumNext();
	while (pCur != NULL) {
		_snprintf(dispString, MAX_PATH, "%s : %s",
			pCur->clientName, pCur->portName);
		gpPSDialog->AddConnectionEntry(dispString);
		pCur = (TcpProtocol *) TcpManager::pMe->Clients.enumNext();
	}
	TcpManager::pMe->Clients.endEnum();
}
////////////////////////////////////////////////////////////////////////

// 
int AuthManager::dId = 0;

int AuthManager::Authenticate(char *pHostName, char *pUserName, char *pPortName)
{
	// return negative value if reject.
	// no authentication action defined in this release.
	RefreshDisplay();
	//
	return dId++;
}

void AuthManager::Leave(int iid)
{
}

