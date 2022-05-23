/***********************************************************************

  PCommXs.h
   Transport module interface definition for NPCOMM
   This file is part of NPCOMM transport module interface.
   $Date: 1999/05/06 00:25:05 $ 
   $Revision: 1.4 $

   Copyright (C) 1999 by Hiroyoshi Kurohara
		All Rights Reserved

Permission to use, copy and distribute of this source code
is hereby granted, provided that the above copyright notice appear 
in all copies.

Hiroyoshi Kurohara disclaims all warranties with regard to this 
software, including all implied warranties of merchantability and 
fitness.
In no event shall Hiroyoshi Kurohara be liable for any special, 
indirect or consequential damages or any damages whatsoever resulting 
from loss of use, data or profits, whether in an action of contract, 
negligence or other tortious action, arising out of or in connection 
with the use or performance of this software.
************************************************************************/

#ifndef _PCOMMXS_H
#define _PCOMMXS_H

//
#ifdef __cplusplus
class PCommTransport
{
public:
	int version;
	enum {INTERFACEVERSION = 201};
	virtual ~PCommTransport() {}
	virtual int Initialize(int trId) = 0;
	virtual int Release() = 0;
	virtual int Dispatch(int Command, int dataSize, void *data) = 0;
	virtual int Control(int Command, void *data) = 0;
};
#endif __cplusplus

// Dispatch commands
#define XDC_OPEN		1
#define XDC_CLOSE		2
#define XDC_CONFIG		3
#define XDC_SEND		4
#define XDC_TXCHAR		5
#define XDC_PORTCTRL	6
//
// include SerialConfig definition for XDC_CONFIG
#include "PComConf.h"
//
// control messages
#define CM_RELOAD_SETTING		1
#define CM_SELECT_TRANSPORT		2
#define CM_DDE					3
#define CM_BASE_END			CM_DDE
//
typedef struct XS_DDE_Rec{
	int command;
	union {
		char *pExecString;
		struct {
			char *pReqItem; // In : item name string
			int format; // In/Out : data format CF_????
			void *pData; // Out : resulting data
			int sizeData; // Out : size of resulting data
		} requestData;
	} ddeData;
} XS_DDE;
#define XS_DDE_EXECUTE		2
#define XS_DDE_REQUEST		3
//////////////

#define BUFSIZE_RXD		4096  // recommended rxd buffer size

#endif _PCOMMXS_H
