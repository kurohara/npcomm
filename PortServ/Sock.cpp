/***********************************************************************
    Sock.cpp
	PortShare version1 protocol processor for PortShare server program.

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
#include <winsock.h>
#include <stdio.h>
#include <process.h>
#include "Sock.h"

#define IAC 255
extern SerialDrive *CreateDevice(char *portName, CommConfig *pIniConf, int *pmodem);

// Create desired protocol processor.
// returns instance of the class that implements TcpProtocol. 
TcpProtocol *TcpProtocol::Create(SOCKET sh)
{
	char iniBuf[32], *pIniBuf = iniBuf;
	ComPacketHead *pHead = (ComPacketHead *) iniBuf;
	unsigned long bFlag;
	int retVal;
	int size = sizeof (ComPacketHead);
	bFlag = 0;
	ioctlsocket(sh, FIONBIO, &bFlag);
	do {
		retVal = recv(sh, pIniBuf, size, MSG_PEEK);
		if (retVal == 0 || retVal == SOCKET_ERROR) {
			return NULL;
		}
	} while (size < retVal) ;

	if (pHead->Command == COM_OPEN) {
		if (pHead->size == sizeof (ComOpenArg) ) {
			TcpProtocol *pProto;
			pProto = new PSProto1(sh);
			return pProto;
		} else {
			// other version of (packet type) PortServ protocol
			//
			return NULL;
		}
	}
	if (iniBuf[0] == IAC) { // may be telnet
		// in future, someone may support telnet protocol
	}
	return NULL;
}

//
// Protocol processor implemantation for 
// PortServ protocol version 1
PSProto1::PSProto1(SOCKET hSock)
{
	sh = hSock;
	//
	unsigned long bFlag = 0;
	ioctlsocket(sh, FIONBIO, &bFlag);
	//
	bFlag = TRUE;
	setsockopt(sh, SOL_SOCKET, SO_KEEPALIVE, (char *) &bFlag, sizeof (int));
	bFlag = TRUE;
	setsockopt(sh,IPPROTO_TCP, TCP_NODELAY, (char *) &bFlag, sizeof (int));
	bFlag = FALSE;
	setsockopt(sh,SOL_SOCKET, SO_OOBINLINE, (char *) &bFlag, sizeof (int));

	pDrive = NULL;

	InitializeCriticalSection(&sockCrst);
#ifndef NOCOPY
	InitializeCriticalSection(&rBufCrst);
#endif
}

PSProto1::~PSProto1()
{
	if (pDrive != NULL)
		delete pDrive;
	DeleteCriticalSection(&sockCrst);
#ifndef NOCOPY
	DeleteCriticalSection(&rBufCrst);
#endif
}

// TcpProtocol implementation
int PSProto1::StartComm(PortAuth *pAuth)
{
	// 
	int retval, res = FALSE;
	int id;
	ComPacket *pPack;

	pPack = (ComPacket *) sockBuf;

	retval = _recv(pPack);
	while (retval) {
		switch (pPack->Command) {
		case COM_TXDATA:
			if (pDrive)
				retval = pDrive->TxData(pPack->size, (char *) pPack->cont);
			break;
		case COM_SETCFG:
			if (pDrive)
				retval = pDrive->SetConfig((CommConfig *) pPack->cont);
			break;
		case COM_TXEMD:
			if (pDrive)
				retval = pDrive->TxEMD(*(short *) pPack->cont);
			break;
		case COM_PCTRL:
			if (pDrive)
				retval = pDrive->PortCtrl(*(int *) pPack->cont);
			break;
		case COM_OPEN: {
			ComOpenArg *pArg = (ComOpenArg *) pPack->cont;
			CommConfig initialConfig;
			int initialModemStat;

			//  pArg->hostName may not contain valid data in this version
#if 0
			strncpy(clientName, pArg->hostName, sizeof (pArg->hostName));
#else
			{
				SOCKADDR_IN sin;
				int nameLen = sizeof (SOCKADDR_IN);
				getpeername(sh, (SOCKADDR *) &sin, &nameLen);
				strcpy(clientName, inet_ntoa(sin.sin_addr));
			}
#endif
			strncpy(portName, pArg->portName, sizeof (pArg->portName));
			// because Authenticate() may use TcpProtocol::portName and
			// TcpProtocol::clientName(for UI), it is safe to prepare both of these
			// string variables before calling it.
			id = pAuth->Authenticate(pArg->hostName, NULL, pArg->portName);
			if (id < 0)
				goto _end_loop;
			pDrive = CreateDevice(pArg->portName, &initialConfig, &initialModemStat);
			if (pDrive == NULL) {
				ReplyOpen(FALSE, &initialConfig, initialModemStat);
				goto _end_loop;
			}
			pDrive->SetUpStream(this);
			bEvTxChar = pArg->bEvTxChar;
			ReplyOpen(TRUE, &initialConfig, initialModemStat);
			StartPortSide();
			break;
					   }
		case COM_CLOSE:
			goto _end_loop;
		default:
			break;
		}
		if (!retval)
			goto _end_loop;
		retval = _recv(pPack);
	}
	res = TRUE;
_end_loop:

	if (pDrive) pDrive->ClosePort();
	ReplyClose();
	retval = WaitForSingleObject(hPortSideThread, 10000/* INFINITE*/);
	if (retval == WAIT_TIMEOUT) {
		TerminateThread(hPortSideThread, 0);
	}
	CloseHandle(hPortSideThread);
	if (pDrive) {
		delete pDrive;
		pDrive = NULL;
	}
	if (id >= 0)
		pAuth->Leave(id);

	return res;
}

unsigned int _stdcall PortSideThread(PSProto1 *pProto)
{
	SerialDrive *pDrive = pProto->pDrive;
	ComPacket cPack;
	int ret;
	cPack.Command = COM_NONE;
	cPack.size = 0;
	pDrive->timeout = 10000; // 10 sec
	// send COM_NONE packet if timeout expired
	// (for ver NPCOMM 1.5 or fewer)
	try {
		while (TRUE) {
			ret = pDrive->Recv();
			if (ret == SerialDrive::RECV_TIMEOUT) {
				if (!pProto->_send(&cPack))
					break;
			}
			else if (ret == SerialDrive::RECV_FAIL)
				break;
		}
	}
	catch (...) {
		pDrive->ClosePort();
	}
	_endthreadex(0);
	return 0;
}

void PSProto1::StartPortSide()
{
	unsigned int thId;
	hPortSideThread = (HANDLE) _beginthreadex(NULL, 0, 
						(unsigned int (_stdcall *)(void *)) PortSideThread,
						(void *) this,
						0, &thId);
}

int PSProto1::ReplyOpen(int bSuccess, CommConfig *pConfig, int modemStat)
{
	char sBuf[sizeof (RepOpenCom) + sizeof (ComPacket)];
	ComPacket *pPack = (ComPacket *) sBuf;
	RepOpenCom *pRep = (RepOpenCom *) pPack->cont;

	pPack->Command = REP_OPEN;
	pPack->size = sizeof (RepOpenCom);
	pRep->result = bSuccess ? PSPROTO_OK : PSPROTO_ERR;
	pRep->modemStat = modemStat;
	pRep->dcbPort = *pConfig;

	return _send(pPack);
}

int PSProto1::ReplyClose()
{
#if 0
	char sBuf[sizeof (Reply) + sizeof (ComPacket)];
	ComPacket *pPack = (ComPacket *) sBuf;
	RepOpenCom *pRep = (Reply *) pPack->cont;
#endif

	return TRUE;
}

// SerialUpStream implementation
int PSProto1::WriteToUp(int size, char *pData) // write to upstream
{
	ComPacket *pPack;
	int sSize;
	int rval = TRUE;

#ifdef NOCOPY
	// if caller agreed about NOCOPY, he may not call
	// this function simultaniously.
	while (size > 0) {
		sSize =  size % MAXSIZE_RXD;
		if (sSize == 0)
			sSize = MAXSIZE_RXD;
		pPack = (ComPacket *) (pData - sizeof(ComPacketHead));
		pPack->Command = COM_RXDATA;
		pPack->size = sSize;
		if (! (rval = _send(pPack) ))
			break;
		size -= sSize;
		pData += sSize;
	}
	return rval;
#else
	// I'm afraid about stack expansion....
	// Is "MAXSIZE_RXD" big enaugh?
#if 0
	char lrecvBuf[MAXSIZE_RXD + sizeof (ComPacket)];
	pPack = (ComPacket *) lrecvBuf;
	pPack->Command = COM_RXDATA;
	while (size > 0) {
		sSize =  size % MAXSIZE_RXD;
		if (sSize == 0)
			sSize = MAXSIZE_RXD;
		memcpy(pPack->cont, pData, sSize);
		pPack->size = sSize;
		if (! (rval = _send(pPack) ))
			break;
		size -= sSize;
		pData += sSize;
	}
#else
	EnterCriticalSection(&rBufCrst);
	pPack = (ComPacket *) recvBuf;
	pPack->Command = COM_RXDATA;
	while (size > 0) {
		sSize =  size % MAXSIZE_RXD;
		if (sSize == 0)
			sSize = MAXSIZE_RXD;
		memcpy(pPack->cont, pData, sSize);
		pPack->size = sSize;
		if (! (rval = _send(pPack) ))
			break;
		size -= sSize;
		pData += sSize;
	}
	LeaveCriticalSection(&rBufCrst);
#endif
	return rval;
#endif

}

int PSProto1::LineBreakNotify()
{
	char sBuf[sizeof (ComPacket)];
	ComPacket *pPack = (ComPacket *) sBuf;
	pPack->Command = COM_EVBREAK;
	pPack->size = 0;

	return _send(pPack);
}

int PSProto1::LineErrorNotify(int errStat)
{
	char sBuf[sizeof (ComPacket) + sizeof (int) ];
	ComPacket *pPack = (ComPacket *) sBuf;
	pPack->Command = COM_EVLERR;
	pPack->size = sizeof (int);
	*((int *) pPack->cont) = errStat;

	return _send(pPack);
}

int PSProto1::EventCharNotify()
{
	char sBuf[sizeof (ComPacket)];
	ComPacket *pPack = (ComPacket *) sBuf;
	pPack->Command = COM_EVEVENT;
	pPack->size = 0;

	return _send(pPack);
}

int PSProto1::TxEmptyNotify()
{
	char sBuf[sizeof (ComPacket)];
	ComPacket *pPack = (ComPacket *) sBuf;
	pPack->Command = COM_EVTXEMP;
	pPack->size = 0;

	return _send(pPack);
}

int PSProto1::TxCharNotify()
{
	if (bEvTxChar) {
		char sBuf[sizeof (ComPacket)];
		ComPacket *pPack = (ComPacket *) sBuf;
		pPack->Command = COM_EVTXCHAR;
		pPack->size = 0;

		return _send(pPack);
	}
	return TRUE;
}

int PSProto1::ModemEventNotify(int eventMask, int modemStat)
{
	char sBuf[sizeof (ComPacket) + sizeof (ComEvModem)];
	ComPacket *pPack = (ComPacket *) sBuf;
	ComEvModem *pEv = (ComEvModem *) pPack->cont;
	pPack->Command = COM_EVMODEM;
	pPack->size = sizeof (ComEvModem);

	pEv->evMask = 0;
	if (eventMask & EV_CTS) {
		pEv->evMask |= MMASK_CTS;
	}
	if (eventMask & EV_DSR) {
		pEv->evMask |= MMASK_DSR;
	}
	if (eventMask & EV_RING) {
		pEv->evMask |= MMASK_RI;
	}
	if (eventMask & EV_RLSD) {
		pEv->evMask |= MMASK_DCD;
	}
	pEv->status = 0;
	pEv->status |= (modemStat & MS_RING_ON ? MODEM_RI : 0);
	pEv->status |= (modemStat & MS_CTS_ON ? MODEM_CTS : 0);
	pEv->status |= (modemStat & MS_DSR_ON ? MODEM_DSR : 0);
	pEv->status |= (modemStat & MS_RLSD_ON ? MODEM_DCD : 0);

	return _send(pPack);
}

// not used in this version.
int PSProto1::TxFlowOff(int bOff) // ask to stop/start TxData
{
	return TRUE;
}

// not a part of communication
//
int PSProto1::ShutdownUp()
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////

int PSProto1::_send(ComPacket *pPack)
{
	int size = sizeof (ComPacketHead) + pPack->size;
	char *pSend = (char *) pPack;
	int sizeSent;
	int ret = TRUE;

	EnterCriticalSection(&sockCrst);
	while (size > 0) {
		sizeSent = send(sh, pSend, size, 0);
		if (sizeSent == SOCKET_ERROR) {
			ret = FALSE;
			break;
		}
		size -= sizeSent;
		pSend += sizeSent;
	}
	LeaveCriticalSection(&sockCrst);

	return ret;
}

int PSProto1::_recv(ComPacket *pPack)
{
	int size;
	char *pRecv = (char *) pPack;
	int ret = TRUE;
	int sizeRead;

	size = sizeof (ComPacketHead);
	while (size > 0) {
		sizeRead = recv(sh, pRecv, size, 0);
		if (sizeRead == 0 || sizeRead == SOCKET_ERROR) {
			ret = FALSE;
			goto _end_recv;
		}
		size -= sizeRead;
		pRecv += sizeRead;
	}
	size = pPack->size;
	if (size > MAXSIZE_TXD) {
		ret = FALSE;
		goto _end_recv;
	}
	while (size > 0) {
		sizeRead = recv(sh, pRecv, size, 0);
		if (sizeRead == 0 || sizeRead == SOCKET_ERROR) {
			ret = FALSE;
			goto _end_recv;
		}
		size -= sizeRead;
		pRecv += sizeRead;
	}

_end_recv:
	return ret;
}
