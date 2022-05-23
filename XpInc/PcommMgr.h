/***********************************************************************

  PcommMgr.h
   Main application manager interface for NPCOMM
   This file is part of NPCOMM transport module interface.
   $Date: 1999/03/21 10:30:47 $ 
   $Revision: 1.2 $

	Copyright (C) 1999 by Hiroyoshi Kurohara
		All Rights Reserved

Permission to use, copy and distribute of this source code
is hereby granted, provided that the above copyright notice appear 
in all copies and no modification is applied to this file.

Hiroyoshi Kurohara disclaims all warranties with regard to this 
software, including all implied warranties of merchantability and 
fitness.
In no event shall Hiroyoshi Kurohara be liable for any special, 
indirect or consequential damages or any damages whatsoever resulting 
from loss of use, data or profits, whether in an action of contract, 
negligence or other tortious action, arising out of or in connection 
with the use or performance of this software.
************************************************************************/
#ifndef _PCOMMMGR_H
#define _PCOMMMGR_H

#include "pcommxs.h"

#define DllExport	__declspec( dllexport )

//////////////////////////////////////////////////////////////

#ifdef __cplusplus
class PCommManager
{
public:
	int npVersionNumber;
	int version;// interface version
	OSVERSIONINFO OSVer;
	HINSTANCE hInstance;
	HWND hWinApp;
	enum {INTERFACEVERSION=200};
	virtual int GetProfileString(char *Section, char *Key, char *Value, int maxLen) = 0;
	virtual int GetProfileInt(char *Section, char *Key, int DefValue) = 0;
	virtual int WriteProfileString(char *Section, char *Key, char *Value) = 0;
	virtual int WriteProfileInt(char *Section, char *Key, int Value) = 0;
	//
	virtual int RegisterTransport(HINSTANCE hModule, PCommTransport *tr, char *name) = 0;
	virtual int DeleteTransport(int trId) = 0;
	//
	virtual int Query(int Command, void *data) = 0;
	virtual int Control(int Command, void *data) = 0;
	//
	virtual int Dispatch(int Command, int DataSize, void *pData) = 0;
	//
};
#endif __cplusplus

// Control commands
#define MCC_UI_CTRL		1 // control code to UI
#define MCC_SHUTDOWN	2 // clean up device portion

// Dispatch commands
#define MDC_WRITE			1 // write Rx data to pseudo device
#define MDC_TXD_NOTIFY		2 // notify Tx data
#define MDC_RXD_NOTIFY		3 // not used
#define MDC_EV_NOTIFY		4
#define MDC_ACKNOWLEDGE		5 // may not used
#define MDC_SETCONFIG		6
#define MDC_SETMODEMSTAT	7 // initialize modem status
#define MDC_DISCONNECT		8
#define MDC_TXFLOW			9 // ask manager to stop/start tx data.
///
// structure for MDC_EV_NOTIFY
typedef struct EventDataRec {
	int eventMask;
	int modemStat;
	int errorStat;
} EventData;
//

// code for MDC_ACKNOWLEDGE
#define MC_ACK_OK		1
#define MC_ACK_ERROR	0

#endif _PCOMMMGR_H
