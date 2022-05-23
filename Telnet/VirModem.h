/***********************************************************************
    VirModem.h
    U.D.S.F module for pseudo modem behavior.

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
//
#ifndef _VIRMODEM_H
#define _VIRMODEM_H

#include "Streams.h"
#include "TelProto.h"
#include "STSock.h"

#define BUFSIZE_VIRMODEM 1024

#define MUID_VIRTUALMODEM 0x0003

#define MAX_SCANNEE	3
typedef struct DialScanneeParamsRec {
	char DialCommand[MAX_PATH];
	char Destination[MAX_PATH];
	int IPPort;
	int bTelnetProto;
} DialScanneeParams;

class StringScan // scan object for shortest match, passive scan
{
	char *pString;
	int iCurPos;
	int lenString;
public:
	StringScan() {pString = NULL; iCurPos = 0; lenString = 0;}
	StringScan(char *pScannee) {pString = strdup(pScannee); iCurPos = 0; lenString = strlen(pString);}
	~StringScan() {if (pString != NULL) free(pString);}
	int scan(int cnext) 
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
	int iscan(int cnext) 
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

class VirtualModem : public STModule
{
	int bEnabled;
	int bConnected;
	DialScanneeParams scannee[MAX_SCANNEE];
	StringScan dialScan[MAX_SCANNEE];
	int numScannee;
	int _lineScanProc(char *pBuf);
	int _promptProc(char *pBuf);
	int _lineEditProc(char cnext);
	void _echo(char *sz);
	void _echo(char *str, int len);
	int _doConnect(char *pLocation);
	int _doConnect(char *pHost, int portNum);
	int _enableTelnetProto(int bEnable);
	// variables for modem behavior
	int bEchoback;
	int bEchoResult;
	char **resCodes;
	char buf_lineEdit[BUFSIZE_VIRMODEM];
	int lineLen;
	CRITICAL_SECTION connCrst;
public:
	VirtualModem();
	~VirtualModem();
	// module specific message ids
	enum {
		// well known messages
		STMESSAGE_ENABLE = INDIVIDUAL_MESSAGE(MUID_VIRTUALMODEM, STIMESSAGE_ENABLE),
		STMESSAGE_CLEARPARAMS =  INDIVIDUAL_MESSAGE(MUID_VIRTUALMODEM, STIMESSAGE_CLEARPARAMS),
		STMESSAGE_ADDPARAM = INDIVIDUAL_MESSAGE(MUID_VIRTUALMODEM, STIMESSAGE_ADDPARAM )
		// specific messages
	};
// streams interfaces
	int DataFromUpper(int size, char *pBuf);
	int DataFromLower(int size, char *pBuf);
	int ControlFromUpper(STMessage *pMes);
	int ControlFromLower(STMessage *pMes);

};

#endif // _VIRMODEM_H