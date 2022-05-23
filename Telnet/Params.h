/***********************************************************************
    Params.h
    The parameters definitions for Telnet.xpm.

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
#ifndef _PARAMS_H
#define _PARAMS_H

#include "STSock.h"
#include "VirModem.h"
#include "ProxProc.h"

typedef struct TelnetParamsRec {
	int bConnectOnDial;
	DialScanneeParams dialParams[MAX_SCANNEE];
	int bUseProxy;
	ProxyParams proxParams;
	RasParams rasParams;
} TelnetParams;

#endif _PARAMS_H