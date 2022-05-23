/***********************************************************************
    PSProto.h
    The PortShare protocol definitions.

	Copyright (C) 1999-2003 by Hiroyoshi Kurohara
		All Rights Reserved

  	This file is part of TcpShare XP module(transport DLL for NPCOMM)
	and PortShare server program.

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
#ifndef _PSPROTO_H
#define _PSPROTO_H

#include "PComConf.h"
#pragma pack(1)

typedef struct ComPacketHeadRec {
	unsigned char Command;
	unsigned short size;
} ComPacketHead;

typedef struct ComPacketRec {
	unsigned char Command;
	unsigned short size;
	unsigned char cont[1];
} ComPacket;

#ifdef BUFSIZE_RXD
#undef BUFSIZE_RXD
#endif

#define MAXSIZE_RXD		4096
#define MAXSIZE_TXD		16384

// Packet commands
#define REPLY_MASK		0x80
#define COM_OPEN		0x01
#define REP_OPEN		(REPLY_MASK | COM_OPEN)
#define COM_CLOSE		0x02
#define REP_CLOSE		(REPLY_MASK | COM_CLOSE)
#define COM_TXDATA		0x03
#define REP_TXDATA		(REPLY_MASK | COM_TXDATA)
#define COM_RXDATA		0x04
#define REP_RXDATA		(REPLY_MASK | COM_RXDATA)
#define COM_SETCFG		0x05
#define REP_SETCFG		(REPLY_MASK | COM_SETCFG)
#define COM_FLOFF		0x06
#define COM_FLON		0x07
#define COM_TXEMD		0x08
#define COM_PCTRL		0x09
#define COM_EVMODEM		0x0A
#define COM_EVREAD		0x0B
#define COM_EVEVENT		0x0C
#define COM_EVTXEMP		0x0D
#define COM_EVLERR		0x0E
#define COM_CLRERR		0x0F
#define COM_EVBREAK		0x10
#define COM_SETTO		0x11
#define COM_SETRTRIGGER	0x12
#define COM_EVTXCHAR	0x13
#define COM_NONE		0x00
#define REP_CLRERR		(REPLY_MASK | COM_CLRERR)
//
// modem control kind
#define MMASK_CTS	0x01
#define MMASK_DSR	0x02
#define MMASK_RI	0x04
#define MMASK_DCD	0x08

#define MODEM_CTS	0x01
#define MODEM_DSR	0x02
#define MODEM_RI	0x04
#define MODEM_DCD	0x08
// error bits
#define LERR_FRAME		0x01
#define LERR_OVERRUN	0x02
#define LERR_RXPARITY	0x04

typedef struct ComOpenArgRec {
	char portName[8];
	char hostName[16];
	unsigned char bEvTxChar;
} ComOpenArg;

typedef struct ReplyRec {
	unsigned char result;
	unsigned short resCode;
} Reply;

typedef struct RepOpenComRec {
	unsigned char result;
	unsigned short resCode;
	int	modemStat;
	CommConfig	dcbPort;
} RepOpenCom;

typedef struct ComEvModemRec {
	unsigned char evMask;
	unsigned char status;
} ComEvModem;

typedef unsigned short ComEMChar;
typedef int	ComPCtrlArg;
#if 0 // obsolete
typedef struct RepClearErrRec {
	int errBits;
	COMSTAT stats;
} RepClearErr;
#endif

#define PSPROTO_OK		(unsigned char)0x0
#define PSPROTO_ERR		(unsigned char)0x80

// following definitions are for UDP datagram
#define UDP_CONNECTREQUEST	0x01
#define UDP_AYT				0x02
#define UDP_STATUSREQ		0x03
#define UDP_ACK				0x11
#define UDP_STATUS			0x12

typedef struct UDPCommandRec {
	char command;
	char data[64];
} UDPCommand;

#define STTYPE_CLIENTS	0x01

typedef struct ServerStatusRec {
	char type;
	char status[63];
} ServerStatus;

#pragma pack ()

#define DEF_SERVICEPORTNUMBER	2345

#endif _PSPROTO_H
