/***********************************************************************
    TcpShare.h
    The main implementation part of TcpShare.xpm.

	Copyright (C) 1999-2003 by Hiroyoshi Kurohara
		All Rights Reserved

  	This file is part of TcpShare XP module(transport DLL for NPCOMM).

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
#ifndef _TCPSHARE_H
#define _TCPSHARE_H

#include "PCommXs.h"

#include "Sock.h"
#define NAME_TRANSPORT "PortShare"
#define XPM_SIGNATURE	"PortShare XPM for TCP/IP ver 1.0 Alpha(protocol 1.0)"

#include "Params.h"
#include "TcpShDlg.h"

class TcpShare : public PCommTransport, public SerialUpStream, public XsUiIntf
{
	int trId;
public:
	PCommManager *pMgr;
	TcpShare();
	~TcpShare();
	// PCommTransport interface
	int Initialize(int trId);
	int Release();
	int Dispatch(int Command, int dataSize, void *data);
	int Control(int Command, void *data);
	//
	// SerialUpStream interface
	int WriteToUp(int size, char *pData);
	int LineBreakNotify();
	int LineErrorNotify(int errStat);
	int EventCharNotify();
	int TxEmptyNotify();
	int TxCharNotify();
	int ModemEventNotify(int eventMask, int modemStat);
	int TxFlowOff(int bOff);
	int ShutdownUp();
	//
	// XsUiIntf 
	int ConnectionTest(int bTouch, int *mStat);
	int getNumPortNames();
	char **getPortNameList();
	void PutParameters(TcpShareParams *newParams);
	void GetParameters(TcpShareParams *newParams);
	void SaveSetting();
	void LoadSetting();
	int isConnected();
	int MgrUIControl(int Command, void *pData);
	int getTransportId();
	//
	void SockMainLoop();
private:
	void StartSockSideThread();
	void buildUpSettingList(char *pSettingString);
	void makeListString(char *pString, int maxLen);

	char *PortNameList[NUM_REMOTEPORTS];
	int NumPorts;
	// cached parameters
	int IPServicePortNum;
	int bTcpCallback;
	char RemoteHostName[MAX_PATH];
	char RemotePortName[32];
	//
	TcpShareParams params;
	CRITICAL_SECTION paramCrst;
	//
	TcpShareDlg *pMySheet;
	//
	int bConnected;
	//
	int currentModemStat;
	CommConfig currentConfig;
	//
	void CacheParameters();
	SerialDrive *pDrive;
	//
	void CancelConnection();

};

// definitions for ini file
#define SECTION_PORTSHARE	"PortShare"

#define KEY_IPPORTNUM		"IPPortNumber"
#define KEY_REMOTEHOST		"RemoteHost"
#define KEY_REMOTEHOST1		"RemoteHost1"
#define KEY_REMOTEHOST2		"RemoteHost2"
#define KEY_REMOTEHOST3		"RemoteHost3"
#define KEY_REMOTEHOST4		"RemoteHost4"
#define KEY_REMOTEHOST5		"RemoteHost5"
#define KEY_REMOTEPORTIND	"RemotePortIndex"
#define KEY_AUTOCHECK		"AutoCheck"
#define KEY_SETTINGLIST		"SETTINGLIST"
#define KEY_TCPCALLBACK		"TCPCallBack"
#define KEY_DOLOG			"Log"
//

#endif _TCPSHARE_H
