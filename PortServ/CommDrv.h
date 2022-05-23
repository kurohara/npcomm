/***********************************************************************
    CommDrv.h
	Communication port driver class for PortShare server program.

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
// CommDrv.h
#ifndef _COMMDRV_H
#define _COMMDRV_H

#include "SerialD.h"

#define SIZE_PREP 256
#define MAXSIZE_RXD		4096
#define MAXSIZE_TXD		4096
class CommDriver : public SerialDrive
{
	char rxBuf[MAXSIZE_RXD
#ifdef NOCOPY
		+ SIZE_PREP
#endif
		];
	char *pRxBuf;
	OVERLAPPED ovlWrite, ovlRecv, ovlEvent;
	DCB curDCB;
	unsigned long evt;
	int bClosed;
protected:
	HANDLE hPort;
public:
	CommDriver();
	~CommDriver();
	SerialUpStream *pUpStream;

	int SetUpStream(SerialUpStream *pUp);

	int Connect(char *hostName, int progNum, void *exData);
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
	int _readData(char *pBuf, int maxSize);

};

#endif _COMMDRV_H
