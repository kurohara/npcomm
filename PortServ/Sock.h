/***********************************************************************
    Sock.h
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
#ifndef _SOCK_H
#define _SOCK_H

#include "PSProto.h"
#include "SerialD.h"

// Used authentication if needed.
class PortAuth
{
public:
	virtual ~PortAuth() {}
	virtual int Authenticate(char *pHostName, char *pUserName, char *pPortName) = 0;
	virtual void Leave(int iid) = 0;
};

class AnyList
{
	AnyList *pNext, *pPrev;
public:
	AnyList()
	{
		pNext = pPrev = NULL;
	};
	AnyList(AnyList *next, AnyList *prev)
	{
		pNext = next; pPrev = prev;
	};
	virtual ~AnyList() {}
	void insertIt(AnyList *pL) // insert to this
	{
		if (pNext != NULL)
			pNext->pPrev = pL;
		pL->pNext = pNext;
		pL->pPrev = this;
		pNext = pL;
	};
	void removeMe() // remove this
	{
		if (pPrev != NULL)
			pPrev->pNext = pNext;
		if (pNext != NULL)
			pNext->pPrev = pPrev;

	};
	AnyList *getNext()
	{
		return pNext;
	};
	AnyList *getPrev()
	{
		return pPrev;
	};
};

// protocol processor interface for TCP
//
class TcpProtocol : public AnyList
{
public:
	virtual ~TcpProtocol() { }
	int clId;
	char clientName[MAX_PATH];
	char portName[MAX_PATH];
	static TcpProtocol *Create(SOCKET sh);
	virtual int StartComm(PortAuth *pAuth) = 0;
};

// version 1 PortServ protocol processor
class PSProto1 : public TcpProtocol, public SerialUpStream
{
	SOCKET sh;
	CRITICAL_SECTION sockCrst; // just for send
	char sockBuf[sizeof(ComPacketHead) + MAXSIZE_TXD];
#ifndef NOCOPY
	CRITICAL_SECTION rBufCrst;
	char recvBuf[sizeof(ComPacketHead) + MAXSIZE_RXD];
#endif
	int bEvTxChar;
	HANDLE hPortSideThread;
	void StartPortSide();
	int ReplyOpen(int bSuccess, CommConfig *pConfig, int modemStat);
	int ReplyClose();
	int _recv(ComPacket *pPack);
public:
	int _send(ComPacket *pPack);
	PSProto1(SOCKET sh);
	~PSProto1();

	SerialDrive *pDrive;

	// TcpProtocol implementation
	int StartComm(PortAuth *pAuth);

	// SerialUpStream implementation
	int WriteToUp(int size, char *pData); // write to upstream
	int LineBreakNotify();
	int LineErrorNotify(int errStat);
	int EventCharNotify();
	int TxEmptyNotify();
	int TxCharNotify();
	int ModemEventNotify(int eventMask, int modemStat);
	int TxFlowOff(int bOff); // ask to stop/start TxData
	// not a part of communication
	int ShutdownUp();
	//
};

#endif _SOCK_H