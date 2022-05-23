/***********************************************************************
    TelProto.h
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
 TelProto.h
 Telnet protocol processor as U.D.S.F module.
 Because from 2 reason, I have not implemented almost of TELNET protocols.
 1. Windows's COM port is not UNIX's tty ( have no line edit behaviro, no terminal informations etc...),
     almost works should goes to terminal applications.
 2. It takes time cost but have no big good effect.
 */

#ifndef _TELPROTO_H
#define _TELPROTO_H

#include "Streams.h"

#define BUFSIZE_TELNET 1024

#define MUID_TELNETPROTO 0x0001

class TelnetProtocol : public STModule
{
	int bActProto;
	// status variables for reading 
	int status;
	int subStatus;
	//
	CRITICAL_SECTION downCrst, upCrst;
	char procBuf_up[BUFSIZE_TELNET], procBuf_down[BUFSIZE_TELNET];

	int _iac_proc(int cnext);
	int _sb_proc(int cnext);
	int _do_proc(int cnext);
	int _dont_proc(int cnext);
	int _will_proc(int cnext);
	int _wont_proc(int cnext);

public:
	TelnetProtocol();
	~TelnetProtocol();
	enum {
		// well known messages
		STMESSAGE_ENABLE = INDIVIDUAL_MESSAGE(MUID_TELNETPROTO, STIMESSAGE_ENABLE)
	};
// streams interfaces
	int DataFromUpper(int size, char *pBuf);
	int DataFromLower(int size, char *pBuf);
	int ControlFromUpper(STMessage *pMes);
	int ControlFromLower(STMessage *pMes);
};

// code definitions for TELNET protocols
#define NORMAL		0x10
#define TEL_OTHER	0x20 // 
#define TEL_SE		240  // end of subnegotiation
#define TEL_NOP		241  //
#define TEL_DATA_MARK	242  //
#define TEL_BREAK	243  //
#define TEL_IP		244  //
#define TEL_AO		245  //
#define TEL_AYT		246  //
#define TEL_EC		247  //
#define TEL_EL		248  //
#define TEL_GA		249  //
#define TEL_SB		250  // subnegotiation will follows
#define TEL_WILL	251  // 
#define TEL_WONT	252  //
#define TEL_DO		253  //
#define TEL_DONT	254  //
#define TEL_IAC		255  // escape code
#define TEL_LOGOUT	18
#define TEL_ECHO	1
#define TEL_BINARY	0

// default ip portnumber for TELNET
#define DEF_TELNETPORT	23

#endif _TELPROTO_H
