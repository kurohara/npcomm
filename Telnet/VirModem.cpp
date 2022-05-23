/***********************************************************************
    VirModem.cpp
    a U.D.S.F module for pseudo modem behavior.

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

// processes dial command scan, etc...
#include "VirModem.h"
#include <stdio.h>

#define RESID_OK	0
#define RESID_ERROR	1
#define RESID_CONNECTED	2
#define RESID_BUSY	3

static char *resultCodes1[] = {"OK\r\n", "ERROR\r\n", "CONNECTED\r\n", "BUSY\r\n"};
static char *resultCodes2[] = {"", "", "", ""};

#define VMODEM_PROMPT "NPCOMM (Please type destination[:port]) >> "
#define BSCHAR	'\b'
#define KILLCHAR '\x015'
#define CRCODE	'\r'
#define LFCODE  '\n'

/////////////////////////////////////////////////
VirtualModem::VirtualModem()
{
	resCodes = resultCodes1;
	bEchoback = TRUE;
	bEchoResult = TRUE;
	bConnected = FALSE;
	//
	lineLen = 0;

	InitializeCriticalSection(&connCrst);

}

VirtualModem::~VirtualModem()
{
	DeleteCriticalSection(&connCrst);

}

/////////////////////////////////////////////////
int VirtualModem::DataFromUpper(int size, char *pBuf)
{
	if (!bEnabled || bConnected)
		return WriteToLower(size, pBuf);
	/////////////////////////////////////
	int i;
	for (i=0;i<size;i++) {
		if (_lineEditProc(*pBuf)) {
			EnterCriticalSection(&connCrst);
			if (numScannee > 0) {
				_lineScanProc(buf_lineEdit);
			} else {
				_promptProc(buf_lineEdit);
			}

			if (!bEnabled || bConnected) {
//				if (size - i > 0)
//					WriteToLower(size-i, pBuf);
				LeaveCriticalSection(&connCrst);
				return size;
			}
			LeaveCriticalSection(&connCrst);
		}
		pBuf++;
	}
	return size;
}

int VirtualModem::DataFromLower(int size, char *pBuf)
{
	return WriteToUpper(size, pBuf);
}

int VirtualModem::ControlFromUpper(STMessage *pMes)
{
	// only upper top user will know which modules are in use
	switch (pMes->type.anyMessage) {
	// module specific messages
	case STMESSAGE_ENABLE:
		bEnabled = (int) pMes->pData;
		return TRUE;
	case STMESSAGE_CLEARPARAMS:
		numScannee = 0;
		return TRUE;
	case STMESSAGE_ADDPARAM:
		if (pMes->pData != NULL && numScannee < MAX_SCANNEE) {
			scannee[numScannee] = *((DialScanneeParams *) pMes->pData) ;
			if (strlen(scannee[numScannee].DialCommand) > 0) {
				dialScan[numScannee].setScannee(scannee[numScannee].DialCommand);
				numScannee++;
			}
			return TRUE;
		} else
			return FALSE;
	// broadcasted messages
	case STMESSAGE_OPEN:
		bConnected = FALSE;
		break;
	case STMESSAGE_CLOSE:
		break;
	case STMESSAGE_CONNECT:
		if (!bEnabled)
			break;
		lineLen = 0;
		// cut control chain here
		return TRUE;
	}
	return ControlLower(pMes);
	//
}

int VirtualModem::ControlFromLower(STMessage *pMes)
{
	// bottom side(driver routines) will not know
	// (should not know) which modules are pushed.

	STMessage aMes;
	// process broadcasted message
	switch (pMes->type.broadcastMessage) {
	case STMESSAGE_DISCONNECTED:
		// re-open lower stream
		EnterCriticalSection(&connCrst);
		_echo("Disconnected.\r\n");
		aMes.type.anyMessage = STMESSAGE_CLOSE;
		ControlLower(&aMes);
		aMes.type.anyMessage = STMESSAGE_OPEN;
		ControlLower(&aMes);
		bConnected = FALSE;
		LeaveCriticalSection(&connCrst);
		break;
#ifdef STMESSAGE_IDLE
	case STMESSAGE_IDLE:
			// show initial prompt if needed
		if (numScannee == 0)
			_echo(VMODEM_PROMPT);
		break;
#endif
	}
	return ControlUpper(pMes);
}

///////////////////////////////////////////////////////////////
int VirtualModem::_lineScanProc(char *pBuf)
{
	int i = 0, j;
	StringScan atScan("AT");
	int isAT = FALSE;

	while (*pBuf != (char) 0) {
		if (i++<2)
			isAT = atScan.iscan(*pBuf);
		// scan user defined dial commands 
		for (j=0;j<numScannee;j++) {
			if (dialScan[j].iscan(*pBuf)) {
				// found entry, reset all scan state

				// found entry, do connect
				_enableTelnetProto(scannee[j].bTelnetProto);
				if (_doConnect(scannee[j].Destination, scannee[j].IPPort)) {
					// echo result code
					_echo(resCodes[RESID_CONNECTED]);
					return TRUE;
				} else {
					// connection failed.
					// echo result code
					_echo(resCodes[RESID_BUSY]);
					return FALSE;
				}
			}
		}
		// check other AT commads if needed
		if (isAT) {
		}
		//
		pBuf++;
	}

	if (isAT) {
		_echo(resCodes[RESID_OK]);
	}
	return FALSE;
}

int VirtualModem::_promptProc(char *pBuf)
{
	char *pHostName, *pIpPort;
	int portNum;
	pHostName = strtok(pBuf, ":\r\n");
	if (pHostName == NULL) {
		_echo(VMODEM_PROMPT);
		return FALSE;
	}
	pIpPort = strtok(NULL, ":\r\n");
	if (pIpPort != NULL) {
		portNum = atoi(pIpPort);
	} else {
		portNum = DEF_TELNETPORT;
	}

	_enableTelnetProto(portNum == DEF_TELNETPORT);
	if (_doConnect(pHostName, portNum)) {
		_echo("\r\nConnected!\r\n");
		return TRUE;
	} else {
		_echo("\r\nConnection failed!\r\n");
		_echo(VMODEM_PROMPT);
		return FALSE;
	}
}

int VirtualModem::_lineEditProc(char cnext)
{
	if (cnext == LFCODE)
		return FALSE;
	if (cnext == BSCHAR) {
		if (lineLen > 0) {
			_echo("\b \b");
			lineLen--;
		}
		return FALSE;
	}
	if (cnext == KILLCHAR) {
		while (lineLen>0) {
			_echo("\b \b");
			lineLen--;
		}
		return FALSE;
	}
	_echo(&cnext, 1);
	buf_lineEdit[lineLen++] = cnext;
	if (lineLen >= BUFSIZE_VIRMODEM - 1 || cnext == CRCODE) {
		buf_lineEdit[lineLen] = (char) 0;
		lineLen = 0;
		return TRUE;
	}
	return FALSE;
}

void VirtualModem::_echo(char *sz)
{
	if (!bEchoback)
		return;
	int len = strlen(sz);
	_echo(sz, len);
}

void VirtualModem::_echo(char *str, int len)
{
	if (!bEchoback)
		return;
	WriteToUpper(len, str);
}

int VirtualModem::_doConnect(char *pHost, int portNum)
{
	char location[MAX_PATH];
	if (pHost && *pHost != (char) 0)
		_snprintf(location, MAX_PATH, "%s:%d", pHost, portNum);
	else
		_snprintf(location, MAX_PATH, "NULL:%d", portNum);

	return _doConnect(location);
}

int VirtualModem::_doConnect(char *pLocation)
{
	STMessage aMes;

	aMes.type.broadcastMessage = STMESSAGE_CONNECT;
	aMes.pData = pLocation;

	return (bConnected = ControlLower(&aMes));

}

int VirtualModem::_enableTelnetProto(int bEnable)
{
	// Yes, I know that lower stream may have TELNET module
	STMessage aMes;

	aMes.type.anyMessage = TelnetProtocol::STMESSAGE_ENABLE;
	aMes.pData = (void *) bEnable;

	return ControlLower(&aMes);

}

