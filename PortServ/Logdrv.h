/************************************************************************
  LogDrv.h

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
#include <windows.h>
#include <stdio.h>

#include "SerialD.h"

class LogDrv : public SerialUpStream, public SerialDrive
{
public:
	SerialUpStream *pUpStream;
	SerialDrive *pDownStream;
	FILE *logFp;
	unsigned int tickDisp;
	LogDrv(SerialDrive *pDown, char *logFile);
	~LogDrv();
	//////////////////////////////////////////////////
	// SerialUpStream implementation
	int WriteToUp(int size, char *pData);
	int LineBreakNotify();
	int LineErrorNotify(int errStat);
	int EventCharNotify();
	int TxEmptyNotify();
	int TxCharNotify();
	int ModemEventNotify(int eventMask, int modemStat);
	int TxFlowOff(int bOff); // ask to stop/start TxData
	// not a part of communication
	int ShutdownUp();

	//////////////////////////////////////////////////
	// SerialDrive implementation
	int SetUpStream(SerialUpStream *pUp);
	// communication
	int Connect(char *hostName, int progNum, void *exData);
	int Disconnect();
	int OpenPort(char *portName, CommConfig *pConfig, int *modemStat);
	int ClosePort();
	int SetConfig(CommConfig *pConfig);
	int TxData(int size, char *pData);
	int TxEMD(short data);
	int PortCtrl(int control);
	// misc
	int SetWindowNotify(_HWND hWnd, int messageId);
	int ClearWindowNotify(_HWND hWnd);
	// this will cause calling callback functions
	int Recv();

};
