/***********************************************************************
    Telnet.h
    The main implementation part of Telnet.xpm.

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
#ifndef _TELNET_H
#define _TELNET_H

#include <Windows.h>
#include "PCommMgr.h"
#include "PCommXs.h"
#include "STSock.h"
#include "VirModem.h"
#include "TelProto.h"
#include "ProxProc.h"

#include "Params.h"
#include "TelnetDlg.h"

#define NAME_TRANSPORT "TELNET"
#define XPM_SIGNATURE "TELNET XPM ver 1.0 Alpha"

//////////////////////////////////////////////////////////////
// 1. "UDSF"(defined in Streams.h) is not a part of NPCOMM transport 
//     interfaces.
// 2. It is possible to directory implement STDriver in this class.
//    But I used STCallback for readability of source code
class Telnet : public PCommTransport, public STCallback, public XsUiIntf
{
	int trId;
public:
	PCommManager *pMgr;
	TelnetDlg *pMySheet;
	//
	Telnet();
	~Telnet();
	// pcommtransport interfaces
	int Initialize(int tid);
	int Release();
	int Dispatch(int Command, int dataSize, void *data);
	int Control(int Command, void *data);
	// STCallback interface
	int ReadDataCB(int size, char *pData);
	int MessageCB(STMessage *pMes);
	//
	//

	//
	// XsUiIntf part
	void PutParameters(TelnetParams *newParams);
	void GetParameters(TelnetParams *newParams);
	void LoadSetting();
	void SaveSetting();
	int MgrUIControl(int Command, void *pData);
	int getTransportId();

	// 
	void NotifyWritten();
	int WriteToDevice(int size, char *pData);
	void SetModemState(int modemStat);
	void NotifyLineBreak();
	void NotifyDisconnect();

private:
	char inBuf[BUFSIZE_RXD];
	int bOpened;
	int currentModemStat;

	CRITICAL_SECTION paramCrst;
	SockStream *pStSock;
	TelnetParams params;
	void CacheParameters();
};

// default integer value for profile data
#define DEFAULT_INT	99
// default portnumber for TELNET protocol
#define DEF_TELNETPORT 23
// definitions for ini file
#define SECTION_TELNET		"Telnet"
#define KEY_USEPROXY		"UseProxy"
#define KEY_PROXYSERVER		"ProxyServer"
#define KEY_PROXYPORT		"ProxyPort"
#define KEY_DIALCOMMAND		"DialCommand"
#define KEY_DIALCOMMAND1	"DialCommand1"
#define KEY_DIALCOMMAND2	"DialCommand2"
#define KEY_DIALCOMMAND3	"DialCommand3"
#define KEY_DESTINATION		"Destination"
#define KEY_DESTINATION1	"Destination1"
#define KEY_DESTINATION2	"Destination2"
#define KEY_DESTINATION3	"Destination3"
#define KEY_SERVERPORT		"ServerPort"
#define KEY_SERVERPORT1		"ServerPort1"
#define KEY_SERVERPORT2		"ServerPort2"
#define KEY_SERVERPORT3		"ServerPort3"
#define KEY_CONNECTONDIAL	"ConnectOnDial"
#define KEY_PROXYRESULT		"ProxyResultStr"
#define KEY_PROXYTIMEOUT	"ProxyWaitTime"
#define KEY_KICKRAS			"KickRas"
#define KEY_RASENTRY		"RASEntry"
//

#endif _TELNET_H
