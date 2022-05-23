/***********************************************************************
    TestDrv.h
	Test module as Communication port driver class for PortShare 
	server program.
    
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
#ifndef _TESTDRV_H
#define _TESTDRV_H

#include "SerialD.h"
#define SIZE_PREP 256
#define MAXSIZE_RXD		4096
#define MAXSIZE_TXD		4096

class StringScan // scan object for shortest match, passive scan
{
	char *pString;
	int iCurPos;
	int lenString;
public:
	StringScan() {pString = NULL; iCurPos = 0; lenString = 0;}
	StringScan(char *pScannee) {pString = strdup(pScannee); iCurPos = 0; lenString = strlen(pString);}
	virtual ~StringScan() {if (pString != NULL) free(pString);}
	virtual int scan(int cnext) 
	{
		if (pString == NULL) return FALSE;
		if (pString[iCurPos] == (char) cnext)
			iCurPos++;
		else
			iCurPos = 0;
		if (iCurPos >= lenString)
			return TRUE;
		return FALSE;
	}
	virtual int iscan(int cnext) 
	{
		
		if (pString == NULL) return FALSE;
		if (toupper(pString[iCurPos]) == toupper(cnext))
			iCurPos++;
		else
			iCurPos = 0;
		if (iCurPos >= lenString)
			return TRUE;
		return FALSE;
	}
	void setScannee(char *pNewString)
	{
		if (pString != NULL) free(pString);
		pString = strdup(pNewString);
		lenString = strlen(pString);
	}
};

class MCScan : public StringScan
{
	int linePos;
	int bIsAt;
public:
	int atX;
	MCScan() : StringScan("AT") {linePos = 0; bIsAt = FALSE;}
	~MCScan() {}
	int iscan(int cnext)
	{
		if (linePos < 2)
			bIsAt = StringScan::iscan(cnext);
		if (bIsAt && linePos == 2)
			atX = toupper(cnext);
		linePos++;
		if (cnext == (int) '\r') {
			linePos = 0;
			if (bIsAt) {
				bIsAt = FALSE;
				return TRUE;
			}
		}
		if (cnext == (int) '\n')
			linePos = 0;
		return FALSE;
	}
};

enum Commands {   LIST,       STARTECHO,       STOPECHO,       SHOWDCB,       SHOWMODEMSTAT,       SETDISCONNECT, ATCOMMAND, ATDIAL } ;

class CommandsScan
{
	StringScan *commands[ATDIAL];
public:
	CommandsScan();
	~CommandsScan();
	int scan(int cnext);

};

class TestDriver : public SerialDrive
{
	char rxBuf[MAXSIZE_RXD
#ifdef NOCOPY
		+ SIZE_PREP
#endif
		];
	char *pRxBuf;
	HANDLE hEndEvent;
	CommandsScan commands;
	int bLineConnected;
	int bEcho;
public:
	TestDriver();
	~TestDriver();
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
	int SetWindowNotify(HWND hWnd, int messageId);
	int ClearWindowNotify(HWND hWnd);
	// this will cause calling callback functions
	int Recv();
private:
	DCB curDCB;
	short lastSentChar;
	int curModemStat;
	void _showDCB();
	void _showModemStat();
	void _showList();
};


#endif _TESTDRV_H
