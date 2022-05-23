/***********************************************************************
    TelProto.cpp
    Telnet protocol processor as U.D.S.F module.

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

/*
 Because from 2 reason, I have not implemented almost of TELNET protocols.
 1. Windows's COM port is not UNIX's tty ( have no line edit behaviro, no terminal informations etc...),
     almost works should goes to terminal applications.
 2. It takes time cost but have no big good effect.
 */

#include <Windows.h>
#include "Telnet.h"
#include "STSock.h"
#include "TelProto.h"

TelnetProtocol::TelnetProtocol()
{
	status = NORMAL;
	subStatus = NORMAL;
	InitializeCriticalSection(&downCrst);
	InitializeCriticalSection(&upCrst);
}

TelnetProtocol::~TelnetProtocol()
{
	DeleteCriticalSection(&downCrst);
	DeleteCriticalSection(&upCrst);
}

///////////////////////////////////////////////////////////
int TelnetProtocol::DataFromUpper(int size, char *pBuf)
{
	int retval=0;
	if (!bActProto)
		return WriteToLower(size, pBuf);

	/////////////////
	EnterCriticalSection(&downCrst);
	int i, nCharToWrite = 0, nWritten;
	for (i=0;i<size;i++) {

		if (*pBuf == TEL_IAC) {
			procBuf_down[nCharToWrite++] = (char) TEL_IAC;
			procBuf_down[nCharToWrite++] = (char) TEL_IAC;
		} else {
			procBuf_down[nCharToWrite++] = *pBuf;
		}
		if (nCharToWrite >= BUFSIZE_TELNET-2) {
			nWritten = WriteToLower(nCharToWrite, procBuf_down);
			if (nWritten == -1) {
				retval = -1;
				goto end_downproc;
			}
			nCharToWrite -= nWritten;
		}
		pBuf++;
	}
	
	nWritten = WriteToLower(nCharToWrite, procBuf_down);
	retval = nWritten > 0 ? size : -1;

end_downproc:

	LeaveCriticalSection(&downCrst);

	return retval;
}

int TelnetProtocol::DataFromLower(int size, char *pBuf)
{
	unsigned char *pVal;
	if (!bActProto)
		return WriteToUpper(size, pBuf);
	/////////////////
	EnterCriticalSection(&upCrst);
	int retval=0;
	int i, nCharToWrite = 0, nWritten;
	for (i=0;i<size;i++) {
		pVal = (unsigned char *) pBuf;
		switch (status) {
		case NORMAL:
			if (*pVal == (unsigned char) TEL_IAC) 
				status = TEL_IAC;
			else
				procBuf_up[nCharToWrite++] = *pBuf;
			break;
		case TEL_IAC:
			if (*pVal == (unsigned char) TEL_IAC) {
				procBuf_up[nCharToWrite++] = (char) TEL_IAC;
				status = NORMAL;
			} else {
				status = _iac_proc((int) *pVal);
			}
			break;
		case TEL_DO:
			status = _do_proc((int) *pVal);
			break;
		case TEL_DONT:
			status = _dont_proc((int) *pVal);
			break;
		case TEL_WILL:
			status = _will_proc((int) *pVal);
			break;
		case TEL_WONT:
			status = _wont_proc((int) *pVal);
			break;
		case TEL_SB:
			status = _sb_proc((int) *pVal);
			break;
		default:
			status = NORMAL;
			break;

		}

		if (nCharToWrite >= BUFSIZE_TELNET) {
			nWritten = WriteToUpper(nCharToWrite, procBuf_up);
			if (nWritten == -1) {
				retval = -1;
				goto end_upproc;
			}
			nCharToWrite -= nWritten;
		}
		pBuf++;
	}
	nWritten = WriteToUpper(nCharToWrite, procBuf_up);
	retval = nWritten > 0 ? size : -1;

end_upproc:

	LeaveCriticalSection(&upCrst);

	return retval;

}

int TelnetProtocol::ControlFromUpper(STMessage *pMes)
{
	switch (pMes->type.anyMessage) {
	// only upper top user will know which modules are in use
	case STMESSAGE_ENABLE:
		bActProto = (int) pMes->pData;
		return TRUE;
	// process broadcasted message
	case STMESSAGE_OPEN:
		break;
	case STMESSAGE_CLOSE:
		break;
	}
	return ControlLower(pMes);
	//
}

int TelnetProtocol::ControlFromLower(STMessage *pMes)
{
	// bottom side(driver routines) will not know
	// (should not know) which modules are pushed.

	// process broadcasted message
	switch (pMes->type.individMessage.messageId) {
	case STMESSAGE_DISCONNECTED:
		break;
#ifdef STMESSAGE_IDLE
	case STMESSAGE_IDLE:
		// send AYT data to lower
		break;
#endif
	}
	return ControlUpper(pMes);
}
///////////////////////////////////////////////////////////

int TelnetProtocol::_iac_proc(int cnext)
{
	switch (cnext) {
	case TEL_SB:
		return TEL_SB;
	case TEL_DO:
		return TEL_DO;
	case TEL_DONT:
		return TEL_DONT;
	case TEL_WILL:
		return TEL_WILL;
	case TEL_WONT:
		return TEL_WONT;
	default:
		return TEL_OTHER;
	}
}

int TelnetProtocol::_sb_proc(int cnext)
{
	switch (subStatus) {
	case NORMAL:
		if (cnext == TEL_IAC)
			subStatus = TEL_IAC;
		return TEL_SB;
	case TEL_IAC:
		if (cnext == TEL_IAC) {
			subStatus = NORMAL;
			return TEL_SB;
		}
		else if (cnext == TEL_SE) {
			subStatus = NORMAL;
			return NORMAL;
		}

	}
	return TEL_SB;
}

int TelnetProtocol::_do_proc(int cnext)
{
	unsigned char ack[3];

	ack[0] = (unsigned char) TEL_IAC;
	switch (cnext) {
	case TEL_LOGOUT:
		//
		//
		ack[1] = (unsigned char) TEL_WILL;
		ack[2] = (unsigned char) cnext;
		WriteToLower(3, (char *) ack);
		break;
	case TEL_BINARY:
		ack[1] = (unsigned char) TEL_WILL;
		ack[2] = (unsigned char) cnext;
		WriteToLower(3, (char *) ack);
		break;
	default:
		ack[1] = (unsigned char) TEL_WONT;
		ack[2] = (unsigned char) cnext;
		WriteToLower(3, (char *) ack);
		break;
	}

	return NORMAL;
}

int TelnetProtocol::_dont_proc(int cnext)
{
	unsigned char ack[3];

	ack[0] = (unsigned char) TEL_IAC;
	ack[1] = (unsigned char) TEL_WONT;
	ack[2] = (unsigned char) cnext;

	WriteToLower(3, (char *) ack);

	return NORMAL;
}

int TelnetProtocol::_will_proc(int cnext)
{
	if (cnext == TEL_LOGOUT) {
	}
	return NORMAL;
}

int TelnetProtocol::_wont_proc(int cnext)
{
	return NORMAL;
}
