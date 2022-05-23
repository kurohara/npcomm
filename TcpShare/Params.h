/***********************************************************************
    Params.h
    The parameters definitions for TcpShare.xpm.

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

typedef struct PSSettingItemRec {
	char *pAlias;
	char *pHostName;
	char *pPortName;
} PSSettingItem;

#define NUM_HOSTNAMES	5
#define MAX_SETTINGS	20
#define NUM_REMOTEPORTS	10
#define PORTNAMELIST	{"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",  "MODEM"}

typedef struct TcpShareParamsRec {
	unsigned int IPServicePortNum;
	int bTcpCallback;
	char RemoteHostName[MAX_PATH];
	char RemoteHostNames[NUM_HOSTNAMES][MAX_PATH];
	char RemotePortName[32];
	int bStartupCheck;
	int bAutoCheck;
	int nSettingItems;
	PSSettingItem settingItems[MAX_SETTINGS];
} TcpShareParams;

#endif _PARAMS_H