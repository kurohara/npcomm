/***********************************************************************
    Sock.cpp
    The PortShare protocol implementation part of TcpShare.xpm.

	Copyright (C) 1999-2003 by Hiroyoshi Kurohara
		All Rights Reserved

  	This file is part of TcpShare XP module(transport DLL for NPCOMM).
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
#include "Sock.h"
#include "PSProto.h"
#include <stdio.h>
#include <stdarg.h>

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
	if (err!=0) {
		WSACleanup(); 
		wVersionRequested = MAKEWORD(1, 1); 
		err = WSAStartup(wVersionRequested,&WsaData);
	}

	return TRUE;
}

static int fillAddress(SOCKADDR_IN *pAddress, char *hostName)
{
	struct hostent *pHost;
	unsigned int taddr;

	taddr = inet_addr(hostName);
	if (taddr == INADDR_NONE) {
		pHost = gethostbyname(hostName);
		if (pHost == NULL) {
			return FALSE;
		}
		memcpy((char *) &pAddress->sin_addr,
			pHost->h_addr, pHost->h_length);
	} else {
		memcpy((char *) &pAddress->sin_addr,
			&taddr, sizeof (int));
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////
ShareSock::ShareSock()
{
	InitializeSockSystem();
	//
	pCallback = NULL;
	timeout = 1000;
	sh = NULL;
	bEvTxChar = FALSE;

}

ShareSock::~ShareSock()
{
}

int ShareSock::_cbConnect(char *hostName, int portNum)
{
	int err;
	int i;
	SOCKET hSockAcc;
	SOCKADDR_IN sin;
	UDPSock uSock;
	int waitingPort;
	int bFlag;

	hSockAcc = socket(PF_INET, SOCK_STREAM, 0);
	if (hSockAcc == INVALID_SOCKET)
		return FALSE;
	bFlag = TRUE;
	setsockopt(hSockAcc, SOL_SOCKET, SO_REUSEADDR, (char *) &bFlag, sizeof (int));
	// bind a name to socket
	ZeroMemory (&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	// in future version, I will change here to 
	// use vacant portnumber(set to 0).
	waitingPort = portNum;
	sin.sin_port = htons ((unsigned short) waitingPort);
	err = bind(hSockAcc, (LPSOCKADDR)&sin, sizeof (sin));
	if (err == SOCKET_ERROR) {
		closesocket(hSockAcc);
		return FALSE;
	}
	err = listen (hSockAcc, 3); // don't be too tight...
	if (err == SOCKET_ERROR) {
		closesocket(hSockAcc);
		return FALSE;
	}

	uSock.servicePortNumber = portNum;
	for (i = 0;i<CONNECTIONRETRY;i++) {
		fd_set rset;
		struct timeval tmo;

		uSock.ConnectRequest(hostName, waitingPort);
		FD_ZERO(&rset);
		FD_SET(hSockAcc, &rset);
		tmo.tv_sec = 0;
		tmo.tv_usec = timeout * 1000; // 
		err = select(1, &rset, NULL, NULL, &tmo);
		if (err == SOCKET_ERROR) {
			closesocket(hSockAcc);
			return FALSE;
		}
		if (err != 0) {
			sh = accept(hSockAcc, NULL, NULL);
			_setupStream();
			closesocket(hSockAcc);
			return TRUE;
		}
	}
	closesocket(hSockAcc);
	return FALSE;
}

int ShareSock::_normConnect(char *hostName, int portNum)
{
	int err;
	unsigned long bFlag;
	SOCKADDR_IN sin, remoteAddr;

	sh = socket(PF_INET, SOCK_STREAM, 0);
	if (sh == INVALID_SOCKET)
		return FALSE;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons ((unsigned short) portNum);
	//
	// force set to "non blocking mode".
	bFlag = 1;
	ioctlsocket(sh, FIONBIO, &bFlag);
	// bind a name to socket
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = 0;
	err = bind(sh, (LPSOCKADDR)&sin, sizeof (sin));
	if (err == SOCKET_ERROR) {
		closesocket(sh);
		return FALSE;
	}
	//
	if (! fillAddress(&remoteAddr, hostName) ) {
		closesocket(sh);
		return FALSE;
	}
	err = connect (sh,
		(PSOCKADDR) & remoteAddr, sizeof (SOCKADDR_IN));

	if (err == SOCKET_ERROR) {
		if ((err = WSAGetLastError()) == WSAEWOULDBLOCK) {
			fd_set wrfd;
			struct timeval tmval;
			tmval.tv_sec = 0;
			tmval.tv_usec = timeout * CONNECTIONRETRY * 1000;

			FD_ZERO(&wrfd);
			FD_SET(sh, &wrfd);
			err = select(1, NULL, &wrfd, NULL, &tmval);
			if (err == 0 || err == SOCKET_ERROR) {
				closesocket(sh);
				return FALSE;
			}
		} else {
			closesocket(sh);
			return FALSE;
		}
	}

	_setupStream();
	return TRUE;
}

int ShareSock::SetUpStream(SerialUpStream *pUp)
{
	pCallback = pUp;
	return TRUE;
}

// destination:
//     hostname[:portNum[,b]]
//
int ShareSock::Connect(char *hostName, int portNum, void *callback)
{
	int bCallback = (int) callback;

	if (bCallback)
		return _cbConnect(hostName, portNum);
	else
		return _normConnect(hostName, portNum);
}

int ShareSock::Disconnect()
{
	shutdown(sh, 2);
	closesocket(sh);
	return TRUE;
}

int ShareSock::OpenPort(char *portName, CommConfig *pConfig, int *modemStat)
{
	char sBuf[sizeof (ComPacketHead) + sizeof (ComOpenArg)];
	ComPacket *pHead = (ComPacket *) sBuf;
	ComOpenArg *pArg = (ComOpenArg *) pHead->cont;
	RepOpenCom *pRep;

	pHead->Command = COM_OPEN;
	pHead->size = sizeof (ComOpenArg);
	strncpy(pArg->portName, portName, sizeof(pArg->portName));
	//
	pArg->bEvTxChar = bEvTxChar;

	if (!_send(pHead))
		return FALSE;

	pHead = (ComPacket *) rxBuf;
	if (!_recv(pHead, MAXSIZE_RXD))
		return FALSE;
	pRep = (RepOpenCom *) pHead->cont;
	if (pRep->result == PSPROTO_ERR)
		return FALSE;

	*pConfig = pRep->dcbPort;
	*modemStat = pRep->modemStat;

	return TRUE;
}

int ShareSock::ClosePort()
{
	char sBuf[sizeof (ComPacketHead)];
	ComPacket *pHead = (ComPacket *) sBuf;
	pHead->Command = COM_CLOSE;
	pHead->size = 0;

	return _send(pHead);

}

int ShareSock::SetConfig(CommConfig *pConfig)
{
	char sBuf[sizeof (ComPacketHead) + sizeof (CommConfig)];
	ComPacket *pHead = (ComPacket *) sBuf;
	CommConfig *pdConfig = (CommConfig *) pHead->cont;
	*pdConfig = *pConfig;
	pHead->Command = COM_SETCFG;
	pHead->size = sizeof(CommConfig);

	return _send(pHead);
}

int ShareSock::TxData(int size, char *pData)
{
	int ret;
#ifdef NOCOPY
	// PCommManager passes the data buffer which have prepended
	// temporary area.
	// Using this breaks independency of this module.
	//
	// reducing memory copy is not for communication performance.
	// it is for save CPU time which NPCOMM consumes.
	ComPacket *pHead = (ComPacket *) (pData - sizeof(ComPacketHead));
#else
	ComPacket *pHead = (ComPacket *) txBuf;
	memcpy(pHead->cont, pData, size);
#endif
	pHead->Command = COM_TXDATA;
	pHead->size = size;

	ret = _send(pHead);
	
	// process TxChar notify locally if specified so
	if (!bEvTxChar && pCallback)
		pCallback->TxCharNotify();

	return ret;
}

int ShareSock::TxEMD(short data)
{
	char sBuf[sizeof (ComPacketHead) + sizeof (ComEMChar)];
	ComPacket *pHead = (ComPacket *) sBuf;
	ComEMChar *pChar = (ComEMChar *) pHead->cont;
	*pChar = (ComEMChar) data;
	pHead->Command = COM_TXEMD;
	pHead->size = sizeof(ComEMChar);

	return _send(pHead);

}

int ShareSock::PortCtrl(int control)
{
	char sBuf[sizeof (ComPacketHead) + sizeof (ComPCtrlArg)];
	ComPacket *pHead = (ComPacket *) sBuf;
	ComPCtrlArg *pCtrl = (ComPCtrlArg *) pHead->cont;
	*pCtrl = (ComPCtrlArg) control;
	pHead->Command = COM_PCTRL;
	pHead->size = sizeof(ComPCtrlArg);

	return _send(pHead);
}
//
int ShareSock::SetWindowNotify(_HWND hWnd, int messageId)
{
	WSAAsyncSelect(sh, hWnd, messageId, FD_READ);
	return TRUE;
}

int ShareSock::ClearWindowNotify(_HWND hWnd)
{
	WSAAsyncSelect(sh, hWnd, 0, 0);
	return TRUE;
}

int ShareSock::Recv()
{
	ComPacket *pP = (ComPacket *) rxBuf;
	int maxSize = MAXSIZE_RXD;
	int result = FALSE;

	if (!_recv(pP, maxSize))
		return RECV_FAIL;
	if (pCallback == NULL)
		return RECV_FAIL;
	switch (pP->Command) {
	case COM_RXDATA:
		result = (pCallback->WriteToUp(pP->size, (char *) pP->cont) >= 0);
		break;
	case COM_EVTXEMP:
		result = pCallback->TxEmptyNotify();
		break;
	case COM_EVTXCHAR:
		result = pCallback->TxCharNotify();
		break;
	case COM_EVMODEM: {
		int mmask, mstat;
		ComEvModem *pM = (ComEvModem *) pP->cont;
		mmask = 0;
		mmask |= (pM->evMask & MMASK_CTS ? EV_CTS : 0);
		mmask |= (pM->evMask & MMASK_DSR ? EV_DSR : 0);
		mmask |= (pM->evMask & MMASK_RI ? EV_RING : 0);
		mmask |= (pM->evMask & MMASK_DCD ? EV_RLSD : 0);
		mstat = 0;
		mstat |= (pM->status & MODEM_CTS ? MS_CTS_ON : 0);
		mstat |= (pM->status & MODEM_DSR ? MS_DSR_ON : 0);
		mstat |= (pM->status & MODEM_RI ? MS_RING_ON : 0);
		mstat |= (pM->status & MODEM_DCD ? MS_RLSD_ON : 0);
		result = pCallback->ModemEventNotify(mmask, mstat);
					  }
		break;
	case COM_EVEVENT:
		result = pCallback->EventCharNotify();
		break;
	case COM_EVBREAK:
		result = pCallback->LineBreakNotify();
		break;
	case COM_EVLERR:
		result = pCallback->LineErrorNotify(*((int *) pP->cont));
		break;
	case REP_CLRERR:
		result = TRUE;
		break;
	default:
		result = TRUE;
	}

	return result ? RECV_SUCCESS : RECV_FAIL;
}

///////////////////////////////////////////////////
int ShareSock::_send(ComPacket *pP)
{
	int size;
	int sizeWritten;
	char *pBuf;

	pBuf = (char *) pP;
	size = pP->size + sizeof (ComPacketHead);

	while (size > 0) {
		sizeWritten = send(sh, pBuf, size, 0);
		if (sizeWritten == SOCKET_ERROR)
			return FALSE;
		size -= sizeWritten;
		pBuf += sizeWritten;
	}

	return TRUE;
}

int ShareSock::_recv(ComPacket *pP, int maxSize)
{
	int size;
	int sizeRead;
	char *pBuf;

	size = sizeof (ComPacketHead);
	pBuf = (char *) pP;

	while (size > 0) {
		sizeRead = recv(sh, pBuf, size, 0);
		if (sizeRead == 0 || sizeRead == SOCKET_ERROR)
			return FALSE;
		size -= sizeRead;
		pBuf += sizeRead;
	}
	size = pP->size;
	if (size > maxSize) size = maxSize;
	while (size > 0) {
		sizeRead = recv(sh, pBuf, size, 0);
		if (sizeRead == 0 || sizeRead == SOCKET_ERROR)
			return FALSE;
		size -= sizeRead;
		pBuf += sizeRead;
	}
////////

	return TRUE;
}

int ShareSock::_setupStream()
{
	int err;
	unsigned long bFlag;

#if 0
	bFlag = TRUE;
	err = setsockopt(sh,IPPROTO_TCP, TCP_NODELAY, (char *) &bFlag, sizeof (int));
#endif
	bFlag = FALSE;
	err = setsockopt(sh,SOL_SOCKET, SO_OOBINLINE, (char *) &bFlag, sizeof (int));
	// force set to "blocking mode".
	bFlag = 0;
	ioctlsocket(sh, FIONBIO, &bFlag);
	// This option may not supported in Winsock.
	bFlag = TRUE;
	setsockopt(sh, SOL_SOCKET, SO_KEEPALIVE, (char *) &bFlag, sizeof (int));
	return TRUE;
}

////////////////////////////////////////////////////////////////////

UDPSock::UDPSock()
{
	pCallback = NULL;
	
	if (! InitializeSockSystem()) {
		sh= NULL;
		return;
	}
	sh = socket(PF_INET, SOCK_DGRAM, 0);
	if (sh == INVALID_SOCKET) {
		sh = NULL;
		return;
	}
	nAddrEntry = 0;
	_bind(0);
}

UDPSock::~UDPSock()
{
	if (sh== NULL) return;
	closesocket(sh);
}

int UDPSock::ConnectRequest(char *hostName, int waitingPort)
{
	UDPCommand pack;
	pack.command = UDP_CONNECTREQUEST;
	// currently, waitingPort is not uset by PortServ
	*((int *) pack.data) = waitingPort;
	return _sendTo(hostName, &pack);

}

int UDPSock::CheckServerPresense(char *hostName, char *portName)
{
	UDPCommand pack;
	pack.command = UDP_STATUSREQ;
	strcpy(checkingServer, hostName);
	strcpy(checkingCommPort, portName);
	lastStat = CHECKCONN;
	return _sendTo(hostName, &pack);

}

int UDPSock::SearchServer()
{
	UDPCommand pack;
	pack.command = UDP_AYT;
	lastStat = SEARCH;
	return _sendTo(NULL, &pack);
}

int UDPSock::StatusRequest(char *hostName)
{
	UDPCommand pack;
	pack.command = UDP_STATUSREQ;
	lastStat = STATUSREQ;
	return _sendTo(hostName, &pack);
}

int UDPSock::SetWindowNotify(HWND hWnd, unsigned int message)
{
	WSAAsyncSelect(sh, hWnd, message, FD_READ);
	return TRUE;
}

int UDPSock::ClearWindowNotify(HWND hWnd)
{
	WSAAsyncSelect(sh, hWnd, 0, 0);
	return TRUE;
}

int UDPSock::Recv()
{
	char address[MAX_PATH];
	UDPCommand pack;

	_recvFrom(&pack, address);
	if (pCallback == NULL)
		return FALSE;
	switch (pack.command) {
	case UDP_ACK:
		pCallback->RServerEntry(address);
		return TRUE;
	case UDP_STATUS: {
		ServerStatus *pStat = (ServerStatus *) pack.data;
		char *pClientName, *pPortName;
		if (pStat->type != STTYPE_CLIENTS)
			return FALSE;
		// get client name and port name
		pClientName = strtok((char *) pStat->status, ",");
		if (pClientName != NULL) {
			pPortName = strtok(NULL, ",");
		} else {
			pPortName = NULL;
		}
		// do required work
		if (lastStat == CHECKCONN) {
			int bFlag;
			if (pClientName != NULL) {
				if (_isMe(pClientName)) {
					// already connected
					pCallback->RConnectable(TRUE);
					return TRUE;
				}
			}
			if (pPortName != NULL) {
				if (strcmp(pPortName, checkingCommPort) == 0)
					bFlag = FALSE;
				else
					bFlag = TRUE;
			} else {
				bFlag = TRUE;
			}
			pCallback->RConnectable(bFlag);
			return TRUE;
		} else
		if (lastStat == STATUSREQ) {
			pCallback->RStatus(address, pClientName, pPortName);
			return TRUE;
		}
					}
	default:
		break;
	}

	return FALSE;
}

int UDPSock::_bind(int portNumber)
{
	SOCKADDR_IN sin;
	int err;

	if (sh == NULL) return FALSE;

	sin.sin_family = AF_INET;
	sin.sin_port = htons (portNumber);
	sin.sin_addr.s_addr = INADDR_ANY; 
	err = bind(sh, (LPSOCKADDR)&sin, sizeof (sin));
	if (err == SOCKET_ERROR) {
		return FALSE;
	}
	
	return TRUE;
}

int UDPSock::_sendTo(char *pAddress, UDPCommand *pData)
{
	SOCKADDR_IN to;
	int err;
	int bBroadCast;

	ZeroMemory(&to, sizeof(SOCKADDR_IN));
	// if address is NULL, broadcast packet
	if (pAddress) {
		bBroadCast = FALSE;
		if (! fillAddress(&to, pAddress) )
			return FALSE;
	} else {
		bBroadCast = TRUE;
		to.sin_addr.s_addr = INADDR_BROADCAST;
	}
	to.sin_port = htons (servicePortNumber);
	to.sin_family = AF_INET;

	setsockopt(sh, SOL_SOCKET, SO_BROADCAST, (char *) &bBroadCast, sizeof (int));
	err = sendto(sh, (char *) pData, sizeof (UDPCommand), MSG_DONTROUTE, 
					(LPSOCKADDR)&to, sizeof (SOCKADDR_IN));

	return err;
}

int UDPSock::_recvFrom(UDPCommand *pData, char *pAddress)
{
	SOCKADDR_IN from;
	int err;
	int fromlen;

	fromlen = sizeof (from);
	err = recvfrom(sh, 
				(char *) pData, sizeof (UDPCommand), 0, 
				(LPSOCKADDR)&from , &fromlen);
	if (err == SOCKET_ERROR)
		return FALSE;

	// convert to address string
	if (pAddress != NULL)
		strcpy(pAddress, inet_ntoa(from.sin_addr) );
	return TRUE;
}

int UDPSock::_isMe(char *pClientName)
{
	SOCKADDR_IN name;
	int namelen;

	if (getsockname(sh, (LPSOCKADDR) &name, &namelen) == SOCKET_ERROR)
		return FALSE;

	if (strcmp(pClientName, inet_ntoa(name.sin_addr)) == 0)
		return TRUE;

	return FALSE;

}

SOCKADDR_IN *UDPSock::getAddress(char *pStrAddress)
{

	return NULL;
}
