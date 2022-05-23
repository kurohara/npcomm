/***********************************************************************
    STSock.cpp
	Socket class as U.D.S.F driver.

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
#include "STSock.h"
#include <process.h>
#include <stdio.h>

void InitializeSockSystem()
{
	WSADATA WsaData;
	WORD wVersionRequested;
	int err;

	wVersionRequested = MAKEWORD(2, 0);
	err = WSAStartup(wVersionRequested, &WsaData);
	if (err != 0) {
		WSACleanup();
		wVersionRequested = MAKEWORD(1, 1);
		err = WSAStartup(wVersionRequested, &WsaData);
	}
}

#include <shlobj.h>
#define DUN_DISPNAME	"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{992CFFA0-F557-101A-88EC-00DD010CCC48}"
static int _kickDun(char *pEntryName)
{
	LPMALLOC		pMalloc;
	HRESULT			hr;
	LPSHELLFOLDER	pDesktop;
	LPITEMIDLIST	pidlDialUp;
	char	mbsDispName[MAX_PATH * 2];
	WCHAR	wcsDispName[MAX_PATH * 2];
	SHELLEXECUTEINFO  sei;

	if(FAILED(SHGetMalloc(&pMalloc)))
		return FALSE;

	if(FAILED(SHGetDesktopFolder(&pDesktop)))
		return FALSE;

	strcpy(mbsDispName, DUN_DISPNAME);
	strcat(mbsDispName, "\\");
	strcat(mbsDispName, pEntryName);

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
					mbsDispName, strlen(mbsDispName) + 1,
					wcsDispName, MAX_PATH);

	hr = pDesktop->ParseDisplayName( NULL,
                 NULL,
                 wcsDispName,
                 NULL,
                 &pidlDialUp,
                 NULL);

	if(SUCCEEDED(hr)) {

		ZeroMemory(&sei, sizeof(sei));
		sei.cbSize     = sizeof(SHELLEXECUTEINFO);
		sei.fMask      = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
		sei.lpIDList   = pidlDialUp;
		sei.nShow      = SW_SHOWDEFAULT;

		ShellExecuteEx(&sei);

		pMalloc->Free(pidlDialUp);
		pDesktop->Release();

		return TRUE;
	}
	pDesktop->Release();

	return FALSE;
}

void _cdecl StartVirtualISR(SockStream *pStDriver)
{
	pStDriver->RxdMainLoop();
	_endthread();
}

/////////////////////////////////////////////////////////////////////
int SockStream::_connectToRemote(char *hostName, int portNumber)
{
	SOCKADDR_IN  remoteAddr;
	unsigned int taddr;
	struct hostent *hostEntry;
	int ret;

	_kickRas();

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons ((unsigned short) portNumber);
	taddr = inet_addr(hostName);
	if (taddr == INADDR_NONE) {
		hostEntry = gethostbyname(hostName);
		if (hostEntry == NULL) {
			return FALSE;
		}
		memcpy((char *) &remoteAddr.sin_addr,
			hostEntry->h_addr, hostEntry->h_length);
	} else {
		memcpy((char *) &remoteAddr.sin_addr,
			&taddr, sizeof (int));
	}

	ret = connect (sh, (PSOCKADDR) & remoteAddr, sizeof (SOCKADDR_IN));
	if (ret = SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			struct timeval tmWait;
			fd_set wfds;
			FD_ZERO(&wfds);
			FD_SET(sh, &wfds);
			tmWait.tv_sec = 0;
			tmWait.tv_usec = timeToWait * 1000;
			ret = select(1, NULL, &wfds, NULL, &tmWait);
			if (ret == 0 || ret == SOCKET_ERROR)
				return FALSE;
		} else {
			return FALSE;
		}
	}

	return TRUE;
}

int SockStream::_newStreamSocket()
{
	int ret;
	SOCKADDR_IN  myAddr;
	unsigned long bFlag;

	sh = socket(PF_INET, SOCK_STREAM, 0);
	if (sh == INVALID_SOCKET) {
		return FALSE;
	}

	myAddr.sin_family = AF_INET;
	myAddr.sin_port = 0;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(sh, (struct sockaddr *) &myAddr, sizeof (myAddr));

	bFlag = TRUE;
	ret = ioctlsocket(sh, FIONBIO, &bFlag);

	// This option may not supported in Winsock.
	bFlag = TRUE;
	setsockopt(sh, SOL_SOCKET, SO_KEEPALIVE, (char *) &bFlag, sizeof (int));

	return TRUE;
}

int SockStream::_kickRas()
{
	if (!rasParams.bKickRas)
		return FALSE;
	// care for WinNT3.51 does not applied yet.
	return _kickDun(rasParams.RasEntryName);
}

//////////////////////////////////////////////////////////////////
int SockStream::RxdMainLoop()
{
	fd_set rfds;
	int ret;
	int nWritten;
	char *pwbuf;
	struct timeval tm;
try {

	while (TRUE) {
		FD_ZERO(&rfds);
		FD_SET(sh, &rfds);
#ifdef STMESSAGE_IDLE
		tm.tm_sec = 0;
		tm.tm_usec = timeToWait * 1000;
		ret = select(1, &rfds, NULL, NULL, &tm);
		if (ret == SOCKET_ERROR)
			break;
		if (ret == 0) { // timeout expired, send IDLE message to upper

		}
#else
		ret = select(1, &rfds, NULL, NULL, NULL);
		if (ret == 0 || ret == SOCKET_ERROR)
			break;
#endif
		ret = recv(sh, inBuf, BUFSIZE_STDRIVER, 0);
		if (ret == 0) { // may disconnected
			break;
		}
		pwbuf = inBuf;
		while (ret > 0) {
			nWritten = WriteToUpper(ret, pwbuf);
			if (nWritten < 0)
				break;
			ret -= nWritten;
			pwbuf += nWritten;
		}
	}

	// postproc
	STMessage aMes;
	aMes.type.broadcastMessage = STMESSAGE_DISCONNECTED;
	ControlUpper(&aMes);
	return TRUE;
}
catch (...) {
	STMessage aMes;
	aMes.type.broadcastMessage = STMESSAGE_SHUTDOWN;
	// I hope this message get to right place
	ControlUpper(&aMes);
	throw;
}

}

/////////////////////////////////////////////////////////////////////
SockStream::SockStream()
{

	SOCKET tmpSh;
	int err;

	// initialize socket system if needed
	tmpSh = socket(PF_INET, SOCK_STREAM, 0);
	if (tmpSh == INVALID_SOCKET) {
		err = WSAGetLastError();
		if (err == WSANOTINITIALISED) {
			InitializeSockSystem();
		}
	} else {
		closesocket(tmpSh);
	}

	//
	pMyCallback = NULL;
}

SockStream::~SockStream()
{
}
/////////////////////////////////////////////////////////
// implementation of driver interfaces
//

// upper end callback for data
int SockStream::ReadDataCB(int size, char *pData)
{
	// write data to pcomm device
	// 
	return (pMyCallback ? pMyCallback->ReadDataCB(size, pData)
		: size);
}

// upper end callback for control
int SockStream::MessageCB(STMessage *pMes)
{
	switch (pMes->type.individMessage.messageId) {
	case STMESSAGE_DISCONNECTED:
		break;
	default:
		break;
	}
	return (pMyCallback ? pMyCallback->MessageCB(pMes)
		: TRUE);
}

// lower end driver routine for data
int SockStream::WriteToMedia(int size, char *pData)
{
	int nWritten;
	while (size > 0) {
		nWritten = send(sh, pData, size, 0);
		if (nWritten == SOCKET_ERROR)
			return -1;
		size -= nWritten;
		pData += nWritten;
	}
	return size;
}

// lower end driver routine for control
int SockStream::ControlMedia(STMessage *pMes)
{
	int result = TRUE;
	switch (pMes->type.individMessage.messageId) {
	case STMESSAGE_OPEN:
		result = _newStreamSocket();
		break;
	case STMESSAGE_CLOSE:
		shutdown(sh, 2);
		closesocket(sh);
		break;
	case STMESSAGE_CONNECT: {
		char *pHostName, *pportNum;
		int portNum = 0;
		if (pMes->pData == NULL) {
			result = FALSE;
			break;
		}
		pHostName = strtok((char *) pMes->pData, ":");
		if (pportNum = strtok(NULL, ":"))
			portNum = atoi(pportNum);
		result = _connectToRemote(pHostName, portNum);
		if (result) {
			_beginthread(
				(void (_cdecl *)(void *))StartVirtualISR,
				0,
				this);
		}
							}
		break;
	case STMESSAGE_DISCONNECT:
		break;
	default:
		break;
	}

	return result;
}

//////////////////////////////////////////////////////////
// driver specific application interfaces
int SockStream::Open(void)
{

	STMessage aMes;

	aMes.type.broadcastMessage = STMESSAGE_OPEN;
	return ControlLower(&aMes);
}

int SockStream::Close(void)
{
	STMessage aMes;
	aMes.type.broadcastMessage = STMESSAGE_CLOSE;
	return ControlLower(&aMes);
}

int SockStream::Connect(char *pHostName, int portNum)
{
	STMessage aMes;
	char location[MAX_PATH];
	_snprintf(location, MAX_PATH, "%s:%d", (pHostName ? pHostName : "NULL"),
			portNum);
	aMes.pData = (void *) location;
	aMes.type.broadcastMessage = STMESSAGE_CONNECT;
	return ControlLower(&aMes);
}

void SockStream::SetRasParams(RasParams *pParam)
{
	rasParams = *pParam;
}
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
