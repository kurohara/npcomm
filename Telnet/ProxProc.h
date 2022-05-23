/***********************************************************************
    ProxProc.h
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


#ifndef _PROXPROC_H
#define _PROXPROC_H

#include "Streams.h"

typedef struct ProxyParamsRec {
	char ProxyServerName[MAX_PATH];
	int ProxyPortNum;
	char ProxyResultString[MAX_PATH];
	int ProxyWaitTime;
} ProxyParams;

#define DEF_PROXYRESULT "connected"
#define DEF_PROXYTIMEOUT 5000

#define BUFSIZE_PROXPROC 1024

// module unique identifier
#define MUID_PROXPROC 0x0002

class ProxyProcess : public STModule
{
	int bEnabled;
	int bConnected;
	ProxyParams params;
	HANDLE hEvent;
	int iScanResult, lenScannee;
	int WaitProxyConnection();
public:
	ProxyProcess();
	~ProxyProcess();
	enum {
		STMESSAGE_ENABLE = INDIVIDUAL_MESSAGE(MUID_PROXPROC, STIMESSAGE_ENABLE),
		STMESSAGE_SETPARAMS = INDIVIDUAL_MESSAGE(MUID_PROXPROC, STIMESSAGE_SETPARAMS)
	};
// streams interfaces
	int DataFromUpper(int size, char *pBuf);
	int DataFromLower(int size, char *pBuf);
	int ControlFromUpper(STMessage *pMes);
	int ControlFromLower(STMessage *pMes);
};

#endif _PROXPROC_H
