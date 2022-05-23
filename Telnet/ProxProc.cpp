/***********************************************************************
    ProxProc.cpp
	proxy processing module for TELNET transport module.

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

#include "ProxProc.h"
#include "STSock.h"
#include <stdio.h>

ProxyProcess::ProxyProcess()
{
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	bConnected = FALSE;
	iScanResult = 0;
}

ProxyProcess::~ProxyProcess()
{
	CloseHandle(hEvent);
}

int ProxyProcess::DataFromUpper(int size, char *pBuf)
{
	return WriteToLower(size, pBuf);
}

int ProxyProcess::DataFromLower(int size, char *pBuf)
{
	if (!bEnabled || bConnected)
		return WriteToUpper(size, pBuf);
	///////////////////////////////////////////

	// process to connect through proxy
	int i;
	for (i=0;i<size; i++) {
		if (params.ProxyResultString[iScanResult] == *pBuf) {
			iScanResult++;
			if (iScanResult >= lenScannee) {
				// tell waiting thread that connection established
				SetEvent(hEvent);
			}
		} else
			iScanResult = 0;

		pBuf++;
	}

	return size;
}

int ProxyProcess::ControlFromUpper(STMessage *pMes)
{
	int ret;
	switch (pMes->type.anyMessage) {
	// only upper top user will know which modules are in use
	case STMESSAGE_ENABLE:
		bEnabled = (int) pMes->pData;
		return TRUE;
	case STMESSAGE_SETPARAMS: {
		ProxyParams *pParam = (ProxyParams *) pMes->pData;
		params = *pParam;
		lenScannee = strlen(params.ProxyResultString);
							   }
		return TRUE;
	// process broadcasted message
	case STMESSAGE_OPEN:
		ResetEvent(hEvent);
		bConnected = FALSE;
		iScanResult = 0;
		break;
	case STMESSAGE_CLOSE:
		break;
	case STMESSAGE_CONNECT: {
		STMessage aMes;
		char connectRequest[MAX_PATH], location[MAX_PATH];

		if (!bEnabled)
			break;
		_snprintf(location, MAX_PATH, "%s:%d",
			(params.ProxyServerName[0] ? params.ProxyServerName : "NULL"),
			params.ProxyPortNum);
		aMes.type.broadcastMessage = STMESSAGE_CONNECT;
		aMes.pData = location;
		// do connect to proxy server
		ret = ControlLower(&aMes);
		if (!ret)
			return ret;
		// tell the desired host name to proxy server
		_snprintf(connectRequest, MAX_PATH, "%s\r\n", (char *) pMes->pData);
			// if proxy server accept the from "destination:port" 
			//_snprintf(connectRequest, MAX_PATH, "%s:%d\r\n", pDesiredDest->pHostName, pDesiredDest->portNumber);
		WriteToLower(strlen(connectRequest), connectRequest);
		// wait for connection process done.
		return WaitProxyConnection();

							}
	}
	return ControlLower(pMes);
	//
}

int ProxyProcess::ControlFromLower(STMessage *pMes)
{
	// bottom side(driver routines) will not know
	// (should not know) which modules are pushed.

	// process broadcasted message
	switch (pMes->type.individMessage.messageId) {
	case STMESSAGE_DISCONNECTED:
		break;
	}
	return ControlUpper(pMes);
}

//////////////////////////////////////////////////////

int ProxyProcess::WaitProxyConnection()
{
	int ret;
	ret = WaitForSingleObject(hEvent, params.ProxyWaitTime);
	if (ret == WAIT_OBJECT_0) {
		bConnected = TRUE;
		return TRUE;
	}
	return FALSE;
}
