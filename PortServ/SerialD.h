/***********************************************************************
    SerialD.h
	Data stream interface for PortShare server program.

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
//
// (Upper end) Application on client ----- Serial port on server (Lower end)
//
#ifndef _SERIALD_H
#define _SERIALD_H
#include "PComConf.h"

// to avoid inconsistency between MFC program and non-MFC program.
// 
typedef void *_HWND;

// Upper side interface
class SerialUpStream
{
public:
	virtual int WriteToUp(int size, char *pData) = 0;
	virtual int LineBreakNotify() = 0;
	virtual int LineErrorNotify(int errStat) = 0;
	virtual int EventCharNotify() = 0;
	virtual int TxEmptyNotify() = 0;
	virtual int TxCharNotify() = 0;
	virtual int ModemEventNotify(int eventMask, int modemStat) = 0;
	virtual int TxFlowOff(int bOff) = 0; // ask to stop/start TxData
	// not a part of communication
	virtual int ShutdownUp() = 0;
};

// Lower side interface
class SerialDrive
{
public:
	int timeout;
	virtual ~SerialDrive() {}
	// initialization
	virtual int SetUpStream(SerialUpStream *pUp) = 0;
	// communication
	virtual int Connect(char *hostName, int progNum, void *exData) = 0;
	virtual int Disconnect() = 0;
	virtual int OpenPort(char *portName, CommConfig *pConfig, int *modemStat) = 0;
	virtual int ClosePort() = 0;
	virtual int SetConfig(CommConfig *pConfig) = 0;
	// returns number of bytes written if succeeded, otherwise negative
	// value is returned.
	virtual int TxData(int size, char *pData) = 0;
	virtual int TxEMD(short data) = 0;
	virtual int PortCtrl(int control) = 0;
	// misc
	virtual int SetWindowNotify(_HWND hWnd, int messageId) = 0;
	virtual int ClearWindowNotify(_HWND hWnd) = 0;
	// invoking this function will cause calling callback functions
	// returns one of following values.
	enum { RECV_SUCCESS, RECV_FAIL, RECV_TIMEOUT };
	virtual int Recv() = 0;
};

#endif _SERIALD_H
