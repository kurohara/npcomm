/***********************************************************************
    CreatDev.cpp
	Driver Chooser for PortShare server program.

    Copyright (C) 1999-2003 by Hiroyoshi Kurohara
        All Rights Reserved

    This file is part of PortShare server program for Windows.

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
// CreatDev.cpp

#include <Windows.h>
#include <stdio.h>
#include "CommDrv.h"
#include "LogDrv.h"

extern int _getProfileInt(const char *pSection, const char *pKey, int defVal);

// prepare suitable driver object here.
// it may not "CommDriver".
SerialDrive *CreateDevice(char *portName, CommConfig *pIniConf, int *pmodem)
{
	SerialDrive *pDrive;
	if (strnicmp(portName, "COM", 3) == 0) {
		pDrive = new CommDriver;
	}
#if 0 //
	else if (strnicmp(portName, "modem", 5) == 0) {

		pDrive = new TapiDriver;
	}
#endif
	if (pDrive) {
		// insert logger if needed
		if (_getProfileInt("General", "Log", 0)) {
			pDrive = new LogDrv(pDrive, "PSLog.txt");
		}
		//
		if (!pDrive->OpenPort(portName, pIniConf, pmodem))
		{
			delete pDrive;
			return NULL;
		}
	}
	return pDrive;
}
