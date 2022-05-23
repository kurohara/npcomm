/***********************************************************************
    Sock.h
    The PortServ protocol processor definitions part of TcpShare.xpm.

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

/*
 Sock.h
 all protocol definitions are implemened these classes.

 serial communication interface.
 */
#ifndef _SOCK_H
#define _SOCK_H

#include "PComConf.h"
#include "PSProto.h"

#include "SerialD.h"

#define NOCOPY
// class of protocol processor.
class ShareSock : public SerialDrive
{
	char rxBuf[MAXSIZE_RXD + sizeof (ComPacketHead)];
#ifndef NOCOPY
	char txBuf[MAXSIZE_TXD + sizeof (ComPacketHead)];
#endif

public:
	ShareSock();
	~ShareSock();
	SerialUpStream *pCallback;
	int bEvTxChar;

	// SerialDrive interfaces
	int SetUpStream(SerialUpStream *pUp);
	int Connect(char *hostName, int portNum, void *callback);
	int Disconnect();
	int OpenPort(char *portName, CommConfig *pConfig, int *modemStat);
	int ClosePort();
	int SetConfig(CommConfig *pConfig);
	int TxData(int size, char *pData);
	int TxEMD(short data);
	int PortCtrl(int control);
	//
	int SetWindowNotify(_HWND hWnd, int messageId);
	int ClearWindowNotify(_HWND hWnd);
	// this will cause calling callback functions
	int Recv();
private:
	SOCKET sh;
	enum {CONNECTIONRETRY = 10};
	unsigned int servicePortNumber;
	int _send(ComPacket *pP);
	int _recv(ComPacket *pP, int maxSize);
	int _setupStream();
	int _cbConnect(char *hostName, int portNum);
	int _normConnect(char *hostName, int portNum);

};

class UDPCallback
{
public:
	virtual void RConnectable(int bConnectable) = 0;
	virtual void RServerEntry(char *serverName) = 0;
	virtual void RStatus(char *serverName, char *clientName, char *portName) = 0;
};

class UDPSock
{
public:
	UDPSock();
	~UDPSock();
	UDPCallback *pCallback;

	int servicePortNumber;
	int waitTimeout;

	enum {CONNREQ, CHECKCONN, SEARCH, STATUSREQ} lastStat;
	int ConnectRequest(char *hostName, int waitingPortNum);
	int CheckServerPresense(char *hostName, char *portName);
	int SearchServer();
	int StatusRequest(char *hostName);
	//

	int SetWindowNotify(HWND hWnd, unsigned int messageId);
	int ClearWindowNotify(HWND hWnd);
	int Recv(); // call me if any data available on socket
private:
	SOCKET sh;
	// not used now
	SOCKADDR_IN *getAddress(char *pStrAddress);
	struct {
		char *pAddress;
		SOCKADDR_IN *pSin;
	} addrTable[10];
	int nAddrEntry;
	//
	char checkingServer[MAX_PATH];
	char checkingCommPort[MAX_PATH];
	//
	int _bind(int localPortNumber);
	int _broadcast(char *pAddress, UDPCommand *pData);
	int _sendTo(char *pAddress, UDPCommand *pData);
	int _recvFrom(UDPCommand *pData, char *pAddress);
	int _isMe(char *pName);
};

#endif _SOCK_H