/***********************************************************************
    STSock.h
    Socket class as U.D.S.F driver

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

#ifndef _STSOCK_H
#define _STSOCK_H

#include "Streams.h"

typedef struct RasParamsRec {
	char RasEntryName[MAX_PATH];
	int bKickRas;
	enum {FORCE_HUNGUP, ASK_HUNGUP, STAY_ONLINE} 
		fHungupOnDisconnect;
} RasParams;

class STCallback
{
public:
	virtual int ReadDataCB(int size, char *pData) = 0;
	virtual int MessageCB(STMessage *pMes) = 0;
};

#define BUFSIZE_STDRIVER	1024

class SockStream : public STDriver
{
	virtual int ReadDataCB(int size, char *pData); // top end
	virtual int MessageCB(STMessage *pMes);        // top end
	virtual int WriteToMedia(int size, char *pData); // bottom end
	virtual int ControlMedia(STMessage *pMes); // bottom end
	//
	int _newStreamSocket();
	int _connectToRemote(char *hostName, int portNumber);
	SOCKET sh;
	char inBuf[BUFSIZE_STDRIVER]; // for Rxd
	RasParams rasParams;
	int _kickRas();
public:
	SockStream();
	~SockStream();
	int Open(void);
	int Close(void);
	int Connect(char *pHostName, int portNum);
	void SetRasParams(RasParams *pParam);
	
	int timeToWait; // in millisec
	int RxdMainLoop();

	STCallback *pMyCallback;
} ;

#define STMESSAGE_SHUTDOWN	0x1000

//typedef struct ConnectMessageRec {
//	char *pHostName;
//	int portNumber;
//} ConnectMessage;

#endif // _STSOCK_H
